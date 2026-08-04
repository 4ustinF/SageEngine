#include "Precompiled.h"
#include "BaseColliderComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
namespace rj = rapidjson;

void BaseColliderComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	if (value.HasMember("Is Trigger"))
	{
		SetIsTrigger(value["Is Trigger"].GetBool());
	}

	if (value.HasMember("Center"))
	{
		const auto& center = value["Center"].GetArray();
		const float x = center[0].GetFloat();
		const float y = center[1].GetFloat();
		const float z = center[2].GetFloat();
		SetCenter(Vector3(x, y, z));
	}
}

void BaseColliderComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	if (mIsTrigger == true)
	{
		SaveBoolToTemplate(compObj, allocator, "Is Trigger", mIsTrigger);
	}

	SaveVector3ToTemplate(compObj, allocator, "Center", mCenter);
}

void BaseColliderComponent ::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
}

void BaseColliderComponent ::Terminate()
{
	mTransformComponent = nullptr;
	mPhysicsObject = nullptr;
}

const Math::Vector3 BaseColliderComponent::GetCenter() const 
{ 
	if (mTransformComponent)
	{
		return mTransformComponent->GetPosition() + mCenter;
	}

	return mCenter; 
}