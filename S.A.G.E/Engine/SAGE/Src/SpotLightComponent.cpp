#include "Precompiled.h"
#include "SpotLightComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(SpotLightComponent, 128);

void SpotLightComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	// TODO: Load in a pointer to a baked light map if one exist.
}

void SpotLightComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{

}

void SpotLightComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	if (mTransformComponent != nullptr)
	{
		OnPositionChangedHandle = mTransformComponent->GetOnPositionChangeDelegate().AddRaw(this, &SpotLightComponent::OnTransformPositionChanged);
		OnRotationChangedHandle = mTransformComponent->GetOnRotationChangeDelegate().AddRaw(this, &SpotLightComponent::OnTransformRotationChanged);
	}
}

void SpotLightComponent::Terminate()
{
	if (mTransformComponent != nullptr)
	{
		mTransformComponent->GetOnPositionChangeDelegate().Remove(OnPositionChangedHandle);
		mTransformComponent->GetOnRotationChangeDelegate().Remove(OnRotationChangedHandle);
		mTransformComponent = nullptr;
	}
}

void SpotLightComponent::DebugUI()
{
	// TODO:
	// Debug view of spotlight
	
	// Inner Spot Angle
	// Outer Spot Angle
	// Range
	// Color
	// Intensity
	// Shadows: bool canCastShadows = true;
}

void SpotLightComponent::OnEnable()
{
	// TODO: Get/Create/Init/Cache a spotlight from the render service.
}

void SpotLightComponent::OnDisable()
{
	// TODO: Return/Terminate/null a spotlight from the render service.
}

void SpotLightComponent::OnTransformPositionChanged(const Vector3& position)
{
	
}

void SpotLightComponent::OnTransformRotationChanged(const Quaternion& rotation)
{
	// TODO: Look into if spot lights can look straight down?
}

#pragma region ---Setters---

void SpotLightComponent::SetPosition(const Vector3& position)
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

void SpotLightComponent::SetDirection(const Vector3& direction)
{
	// TODO:
}

void SpotLightComponent::SetInnerConeAngle(float innerConeAngle)
{
	mInnerConeAngle = innerConeAngle;
	// TODO:
	// mInnerConeAngle * Constants::DegToRad 
}

void SpotLightComponent::SetOuterConeAngle(float outerConeAngle)
{
	mOuterConeAngle = outerConeAngle;
	// TODO:
	// mOuterConeAngle * Constants::DegToRad 
}

void SpotLightComponent::SetRange(float range)
{
	mRange = range;
	// TODO:
}

void SpotLightComponent::SetAttenuation(const Vector3& attenuation)
{
	mAttenuation = attenuation;
	// TODO:
}

void SpotLightComponent::SetAttenuationConstantTerm(float constantTerm)
{
	mAttenuation.x = constantTerm;
	// TODO:
}

void SpotLightComponent::SetAttenuationLinearTerm(float linearTerm)
{
	mAttenuation.y = linearTerm;
	// TODO:
}

void SpotLightComponent::SetAttenuationQuadraticTerm(float quadraticTerm)
{
	mAttenuation.z = quadraticTerm;
	// TODO:
}

void SpotLightComponent::SetAmbientColor(const Color& color)
{
	mAmbientColor = color;
	// TODO:
}

void SpotLightComponent::SetDiffuseColor(const Color& color)
{
	mDiffuseColor = color;
	// TODO:
}

void SpotLightComponent::SetSpecularColor(const Color& color)
{
	mSpecularColor = color;
	// TODO:
}

#pragma endregion
