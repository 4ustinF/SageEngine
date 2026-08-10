#include "Precompiled.h"
#include "BaseColliderComponent.h"

#include "GameWorld.h"
#include "GameObject.h"
#include "RBPhysicsService.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;
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
	GameObject& ownerGO = GetOwner();
	mTransformComponent = ownerGO.GetComponent<TransformComponent>();
	mRigidBodyComponent = ownerGO.GetComponent<RigidBodyComponent>();
	mPhysicsService = ownerGO.GetWorld().GetService<RBPhysicsService>();
}

void BaseColliderComponent ::Terminate()
{
	mPhysicsObject = nullptr;
	mTransformComponent = nullptr;
	mRigidBodyComponent = nullptr;
	mPhysicsService = nullptr;
}

void BaseColliderComponent::OnEnable()
{
	if(mRigidBodyComponent == nullptr)
	{
		mRigidBodyComponent = GetOwner().GetComponent<RigidBodyComponent>();
	}

	if(mPhysicsObject != nullptr)
	{
		UpdatePhysicsObjectPropertys(); 
		mPhysicsObject = mPhysicsService->GetPhysicsWorld().AddObject(*mPhysicsObject, mRigidBodyComponent ? 
			(mRigidBodyComponent->IsKinematic() ? PhysicsObjectType::Kinematic : PhysicsObjectType::Dynamic) : PhysicsObjectType::Static);
	}
}

void BaseColliderComponent::OnDisable()
{
	if (mPhysicsObject != nullptr)
	{
		mPhysicsService->GetPhysicsWorld().RemoveObject(*mPhysicsObject);
	}
}

const Vector3 BaseColliderComponent::GetCenter() const 
{ 
	if (mTransformComponent)
	{
		return mTransformComponent->GetPosition() + mCenter;
	}

	return mCenter; 
}

const Quaternion BaseColliderComponent::GetOrientation() const
{
	if (mTransformComponent)
	{
		return mTransformComponent->GetRotation();// *mOrientation;
	}

	return mOrientation;
}

void BaseColliderComponent::UpdatePhysicsObjectPropertys()
{
	if (mRigidBodyComponent != nullptr)
	{
		mPhysicsObject->SetMass(mRigidBodyComponent->GetMass());
		mPhysicsObject->SetDrag(mRigidBodyComponent->GetDrag());
		mPhysicsObject->SetAngularDrag(mRigidBodyComponent->GetAngularDrag());
		mPhysicsObject->SetUseGravity(mRigidBodyComponent->GetUseGravity());
	}
	else
	{
		mPhysicsObject->SetMass(0.0f);
		mPhysicsObject->SetDrag(0.0f);
		mPhysicsObject->SetAngularDrag(0.05f);
		mPhysicsObject->SetUseGravity(true);
	}
}