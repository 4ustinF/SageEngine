#include "Precompiled.h"
#include "SpotlightComponent.h"

#include "GameObject.h"
#include "GameWorld.h"
#include "RenderService.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(SpotlightComponent, 128);

void SpotlightComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	if (value.HasMember("Depth Map Resolution"))
	{
		SetDepthMapResolution(value["Depth Map Resolution"].GetString());
	}

	// TODO:
	//LightMode mLightMode = LightMode::PseudoBaked;
	//bool mCanCastShadows = true;
	// Load in a pointer to a baked light map if one exist.

	if (value.HasMember("Inner Cone Angle"))
	{
		SetInnerConeAngle(value["Inner Cone Angle"].GetFloat());
	}

	if (value.HasMember("Outer Cone Angle"))
	{
		SetOuterConeAngle(value["Outer Cone Angle"].GetFloat());
	}

	if (value.HasMember("Range"))
	{
		SetRange(value["Range"].GetFloat());
	}
	
	//void SetAttenuation(const Math::Vector3 & attenuation);
	//void SetAttenuationConstantTerm(float constantTerm);	// Doesn't involve distance at all. It is just added flatly regardless of distance.
	//void SetAttenuationLinearTerm(float linearTerm);		// Falloff proportional to distance. A straight, gentle fade.
	//void SetAttenuationQuadraticTerm(float quadraticTerm);	// Falloff proportional to distance².
	
	//void SetAmbientColor(const Graphics::Color & color);
	//void SetDiffuseColor(const Graphics::Color & color);
	//void SetSpecularColor(const Graphics::Color & color);
}

void SpotlightComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	const int depthMapCount = static_cast<int>(DepthMapResolutionValues.size());
	for (int depthMapIndex = 0; depthMapIndex < depthMapCount; ++depthMapIndex)
	{
		const DepthMapResolution depthMapResolution = DepthMapResolutionValues[depthMapIndex];
		if (depthMapResolution == mDepthMapResolution)
		{
			SaveStringToTemplate(compObj, allocator, "Depth Map Resolution", DepthMapResolutionNames[depthMapIndex]);
			break;
		}
	}
}

void SpotlightComponent::Initialize()
{
	GameObject& owner = GetOwner();
	GameWorld& world = owner.GetWorld();

	mRenderService = world.GetService<RenderService>();
	mTransformComponent = owner.GetComponent<TransformComponent>();

	if (mTransformComponent)
	{
		mSpotLightData.position = mTransformComponent->GetPosition();
		mSpotLightData.direction = mTransformComponent->GetRotation().Rotate(Vector3::ZAxis);
	}
}

void SpotlightComponent::Terminate()
{
	mRenderService = nullptr;
	mTransformComponent = nullptr;
}

void SpotlightComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Spotlight Component##SpotLightComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Text("Resolution: "); ImGui::SameLine();
		int currentResolution = static_cast<int>(std::log2(static_cast<int>(mDepthMapResolution) >> 8));
		if (ImGui::Combo("##Resolution", &currentResolution, DepthMapResolutionNames, IM_ARRAYSIZE(DepthMapResolutionNames)))
		{
			const DepthMapResolution currentDepthMapResolution = static_cast<DepthMapResolution>(256 << currentResolution);
			SetDepthMapResolution(currentDepthMapResolution);
		}

		ImGui::Text("Light Mode: "); ImGui::SameLine();
		int currentLightMode = static_cast<int>(mLightMode);
		if (ImGui::Combo("##LightMode", &currentLightMode, LightModeNames, IM_ARRAYSIZE(LightModeNames)))
		{
			const LightMode currentLightModeEnum = static_cast<LightMode>(currentLightMode);
			SetLightMode(currentLightModeEnum);
		}

		bool canCastShadows = mCanCastShadows;
		if (ImGui::Checkbox("Cast Shadows##SpotLightComponent", &canCastShadows))
		{
			SetCanCastShadows(canCastShadows);
		}

		if (ImGui::DragFloat("Range", &mSpotLightData.range, 0.5f, 1.0f, 500.0f))
		{
			mSpotShadowEffect.Invalidate();
		}

		float innerDeg = mSpotLightData.innerConeAngle * Constants::RadToDeg;
		float outerDeg = mSpotLightData.outerConeAngle * Constants::RadToDeg;
		if (ImGui::DragFloat("Inner Cone (deg)", &innerDeg, 0.5f, 1.0f, outerDeg)) 
		{
			mSpotLightData.innerConeAngle = innerDeg * Constants::DegToRad;
			mSpotShadowEffect.Invalidate();
		}
		if (ImGui::DragFloat("Outer Cone (deg)", &outerDeg, 0.5f, innerDeg, 90.0f)) 
		{
			mSpotLightData.outerConeAngle = outerDeg * Constants::DegToRad;
			mSpotShadowEffect.Invalidate();
		}

		ImGui::ColorEdit4("Ambient", &mSpotLightData.ambient.r);
		ImGui::ColorEdit4("Diffuse", &mSpotLightData.diffuse.r);
		ImGui::ColorEdit4("Specular", &mSpotLightData.specular.r);
		ImGui::DragFloat3("Attenuation (const/lin/quad)", &mSpotLightData.attenuation.x, 0.001f, 0.0f, 2.0f);

		ImGui::Text("Shadow Map");
		ImGui::Image(mSpotShadowEffect.GetDepthMap().GetRawData(), { 144, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1 });

		if (ImGui::Button("Invalidate"))
		{
			mSpotShadowEffect.Invalidate();
		}

		//if (ImGui::Button("SaveSRVToDDS"))
		//{
		//	bool saved = TextureBaking::SaveSRVToDDS(
		//		GraphicsSystem::Get()->GetContext(),
		//		mSpotShadowEffects[i].GetDepthMap().GetShaderResourceView(),
		//		L"D:/GitHubFiles/SageEngine/S.A.G.E/Assets/Baked/SpotShadow0.dds"
		//	);
		//}

		//if (ImGui::Button("LoadDDSAsSRV"))
		//{
		//	//SAGE::Graphics::Texture mBakedSpotShadowMap;
		//	//bool mBakedSpotShadowMapLoaded = true;

		//	//ID3D11ShaderResourceView* srv = TextureBaking::LoadDDSAsSRV(GraphicsSystem::Get()->GetDevice(), L"D:/GitHubFiles/SageEngine/S.A.G.E/Assets/Baked/SpotShadow0.dds");

		//	//if (srv != nullptr)
		//	//{
		//	//	if (mBakedSpotShadowMapLoaded) {
		//	//		mBakedSpotShadowMap.Terminate(); // release the previous one first if reloading
		//	//	}
		//	//	mBakedSpotShadowMap.InitializeFromSRV(srv);
		//	//	mBakedSpotShadowMapLoaded = true;
		//	//	mStandardEffect.SetSpotShadowMap(i, &mBakedSpotShadowMap);
		//	//}
		//}

		SimpleDraw::AddCone(mSpotLightData.position, mSpotLightData.direction, mSpotLightData.outerConeAngle, mSpotLightData.range, 16, Colors::Green);
		SimpleDraw::AddCone(mSpotLightData.position, mSpotLightData.direction, mSpotLightData.innerConeAngle, mSpotLightData.range, 16, Colors::Green);
	}

	// TODO:
	// Shadows: bool canCastShadows = true;
}

void SpotlightComponent::OnEnable()
{
	if (mRenderService)
	{
		mIsRegisteredWithRenderService = mRenderService->RegisterSpotLight(this);
	}

	if (mTransformComponent != nullptr)
	{
		OnPositionChangedHandle = mTransformComponent->GetOnPositionChangeDelegate().AddRaw(this, &SpotlightComponent::OnTransformPositionChanged);
		OnRotationChangedHandle = mTransformComponent->GetOnRotationChangeDelegate().AddRaw(this, &SpotlightComponent::OnTransformRotationChanged);
	}
}

void SpotlightComponent::OnDisable()
{
	if (mRenderService)
	{
		mRenderService->UnregisterSpotLight(this);
		mIsRegisteredWithRenderService = false;
	}

	if (mTransformComponent != nullptr)
	{
		mTransformComponent->GetOnPositionChangeDelegate().Remove(OnPositionChangedHandle);
		mTransformComponent->GetOnRotationChangeDelegate().Remove(OnRotationChangedHandle);
	}
}

void SpotlightComponent::Invalidate()
{
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::OnTransformPositionChanged(const Vector3& position)
{
	mSpotLightData.position = position;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::OnTransformRotationChanged(const Quaternion& rotation)
{
	mSpotLightData.direction = rotation.Rotate(Vector3::ZAxis);
	mSpotShadowEffect.Invalidate();
}

#pragma region ---Setters---

void SpotlightComponent::SetPosition(const Vector3& position)
{
	if (mTransformComponent != nullptr)
	{
		mTransformComponent->SetPosition(position);
	}
	else
	{
		OnTransformPositionChanged(position);
	}
}

//void SpotlightComponent::SetDirection(const Vector3& direction)
//{
//
//}

void SpotlightComponent::SetInnerConeAngle(float innerConeAngle)
{
	mSpotLightData.innerConeAngle = innerConeAngle * Constants::DegToRad;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::SetOuterConeAngle(float outerConeAngle)
{
	mSpotLightData.outerConeAngle = outerConeAngle * Constants::DegToRad;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::SetRange(float range)
{
	mSpotLightData.range = range;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::SetAttenuation(const Vector3& attenuation)
{
	mSpotLightData.attenuation = attenuation;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::SetAttenuationConstantTerm(float constantTerm)
{
	mSpotLightData.attenuation.x = constantTerm;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::SetAttenuationLinearTerm(float linearTerm)
{
	mSpotLightData.attenuation.y = linearTerm;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::SetAttenuationQuadraticTerm(float quadraticTerm)
{
	mSpotLightData.attenuation.z = quadraticTerm;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::SetAmbientColor(const Color& color)
{
	mSpotLightData.ambient = color;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::SetDiffuseColor(const Color& color)
{
	mSpotLightData.diffuse = color;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::SetSpecularColor(const Color& color)
{
	mSpotLightData.specular = color;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::SetDepthMapResolution(const std::string& depthMapResolution)
{
	const int depthMapCount = static_cast<int>(DepthMapResolutionValues.size());
	for (int depthMapIndex = 0; depthMapIndex < depthMapCount; ++depthMapIndex)
	{
		if (DepthMapResolutionNames[depthMapIndex] == depthMapResolution)
		{
			SetDepthMapResolution(DepthMapResolutionValues[depthMapIndex]);
			break;
		}
	}
}

void SpotlightComponent::SetDepthMapResolution(DepthMapResolution depthMapResolution)
{
	if (mDepthMapResolution == depthMapResolution)
	{
		return;
	}

	mDepthMapResolution = depthMapResolution;
	if (mIsRegisteredWithRenderService && mRenderService)
	{
		mRenderService->UnregisterSpotLight(this);
		mIsRegisteredWithRenderService = mRenderService->RegisterSpotLight(this);
	}
}

void SpotlightComponent::SetLightMode(LightMode lightMode)
{
	if (mLightMode == lightMode)
	{
		return;
	}

	// TODO: Light mode currently doesn't do anything.
	mLightMode = lightMode;
}

void SpotlightComponent::SetCanCastShadows(bool castShadows)
{
	// TODO: Can Cast Shadows currently doesn't do anything.
	mCanCastShadows = castShadows;
}

#pragma endregion
