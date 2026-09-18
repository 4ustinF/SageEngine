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