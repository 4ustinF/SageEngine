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
				if (mRenderService)
				{
					mRenderService->UnregisterSpotLight(this);
					mIsSlotIndexValid = mRenderService->RegisterSpotLight(this);
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
		mIsSlotIndexValid = mRenderService->RegisterSpotLight(this);
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
	if (mIsSlotIndexValid)
	{
		mRenderService->GetSpotLight(mSlotIndex).position = position;
		mRenderService->GetSpotShadowEffect(mSlotIndex).Invalidate();
	}
}

void SpotlightComponent::OnTransformRotationChanged(const Quaternion& rotation)
{
	// TODO: Look into if spot lights can look straight down?
}

void SpotlightComponent::InvalidateSpotLight()
{
	if (!mIsSlotIndexValid)
	{
		return;
	}

	mRenderService->GetSpotShadowEffect(mSlotIndex).Invalidate();
}

#pragma region ---Getters---

bool SpotlightComponent::GetCanMarkClean() const
{
	switch (mLightMode)
	{
		case LightMode::RealTime:
			return false;
		case LightMode::PseudoBaked:
		case LightMode::Baked:
			return true;
	}

	return false;
}

//const Vector3& SpotLightComponent::GetPosition()
//{
//	if (mTransformComponent != nullptr)
//	{
//		return mTransformComponent->GetPosition();
//	}
//
//	// TODO: Check if we have a cached spot light and if so return its position.
//
//	return Vector3::Zero; // TODO:
//}
//
//const Vector3& SpotLightComponent::GetDirection()
//{
//	return Vector3::One; // TODO:
//}

#pragma endregion

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
	// TODO:
}

void SpotlightComponent::SetInnerConeAngle(float innerConeAngle)
{
	//mInnerConeAngle = innerConeAngle;
	// TODO:
	// mInnerConeAngle * Constants::DegToRad 
}

void SpotlightComponent::SetOuterConeAngle(float outerConeAngle)
{
	//mOuterConeAngle = outerConeAngle;
	// TODO:
	// mOuterConeAngle * Constants::DegToRad 
}

void SpotlightComponent::SetRange(float range)
{
	//mRange = range;
	// TODO:
}

void SpotlightComponent::SetAttenuation(const Vector3& attenuation)
{
	//mAttenuation = attenuation;
	// TODO:
}

void SpotlightComponent::SetAttenuationConstantTerm(float constantTerm)
{
	//mAttenuation.x = constantTerm;
	// TODO:
}

void SpotlightComponent::SetAttenuationLinearTerm(float linearTerm)
{
	//mAttenuation.y = linearTerm;
	// TODO:
}

void SpotlightComponent::SetAttenuationQuadraticTerm(float quadraticTerm)
{
	//mAttenuation.z = quadraticTerm;
	// TODO:
}

void SpotlightComponent::SetAmbientColor(const Color& color)
{
	//mAmbientColor = color;
	// TODO:
}

void SpotlightComponent::SetDiffuseColor(const Color& color)
{
	//mDiffuseColor = color;
	// TODO:
}

void SpotlightComponent::SetSpecularColor(const Color& color)
{
	//mSpecularColor = color;
	// TODO:
}

#pragma endregion
