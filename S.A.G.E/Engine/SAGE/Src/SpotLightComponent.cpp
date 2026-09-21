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
	// TODO: Load in a pointer to a baked light map if one exist.
}

void SpotlightComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{

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
		//mSpotLightData.direction = mTransformComponent->GetRotation(); // TODO:
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
			DepthMapResolution currentDepthMapResolution = static_cast<DepthMapResolution>(256 << currentResolution);
			if (mDepthMapResolution != currentDepthMapResolution)
			{
				mDepthMapResolution = currentDepthMapResolution;
				if (mIsRegisteredWithRenderService && mRenderService)
				{
					mRenderService->UnregisterSpotLight(this);
					mIsRegisteredWithRenderService = mRenderService->RegisterSpotLight(this);
				}
			}
		}

		ImGui::Text("Light Mode: "); ImGui::SameLine();
		int currentLightMode = static_cast<int>(mLightMode);
		if (ImGui::Combo("##LightMode", &currentLightMode, LightModeNames, IM_ARRAYSIZE(LightModeNames)))
		{
			const LightMode currentLightModeEnum = static_cast<LightMode>(currentLightMode);
			if (mLightMode != currentLightModeEnum)
			{
				mLightMode = currentLightModeEnum;
			}
		}

		if (mIsRegisteredWithRenderService)
		{
			ImGui::Text("Shadow Map");
			ImGui::Image(mSpotShadowEffect.GetDepthMap().GetRawData(), { 144, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1 });
		}
	}

	// TODO:
	// Debug view of spotlight
	
	// Inner Spot Angle
	// Outer Spot Angle
	// Range
	// Color
	// Intensity
	// Shadows: bool canCastShadows = true;
}

void SpotlightComponent::OnEnable()
{
	// TODO: Get/Create/Init/Cache a spotlight from the render service.
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

void SpotlightComponent::OnTransformPositionChanged(const Vector3& position)
{
	mSpotLightData.position = position;
	mSpotShadowEffect.Invalidate();
}

void SpotlightComponent::OnTransformRotationChanged(const Quaternion& rotation)
{
	// TODO: Look into if spot lights can look straight down?
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

void SpotlightComponent::SetDirection(const Vector3& direction)
{
	mSpotLightData.direction = direction;
	mSpotShadowEffect.Invalidate();
}

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

#pragma endregion
