#include "Precompiled.h"
#include "BaseColliderComponent.h"

#include "GameWorld.h"
#include "GameObject.h"
#include "RBPhysicsService.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "MeshFilterComponent.h"

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

void BaseColliderComponent::Initialize()
{
	GameObject& ownerGO = GetOwner();
	mTransformComponent = ownerGO.GetComponent<TransformComponent>();
	mRigidBodyComponent = ownerGO.GetComponent<RigidBodyComponent>();
	mMeshFilterComponent = ownerGO.GetComponent<MeshFilterComponent>();
	mPhysicsService = ownerGO.GetWorld().GetService<RBPhysicsService>();
}

void BaseColliderComponent::Terminate()
{
	if (mPhysicsObject)
	{
		mPhysicsObject->GetOnTriggerEnterDelegate().Remove(OnTriggerEnterHandle);
		mPhysicsObject->GetOnTriggerStayDelegate().Remove(OnTriggerStayHandle);
		mPhysicsObject->GetOnTriggerExitDelegate().Remove(OnTriggerExitHandle);
	}

	mPhysicsObject = nullptr;
	mTransformComponent = nullptr;
	mRigidBodyComponent = nullptr;
	mMeshFilterComponent = nullptr;
	mPhysicsService = nullptr;
}

void BaseColliderComponent::Update(float deltaTime)
{
	// TODO: Get better method then doing this constantly in update. We should only update when a value is changed.
	if (mPhysicsObject)
	{
		if (mPhysicsObject->GetIsStatic()) // Static/Kinematic
		{
			mPhysicsObject->SetPosition(GetCenter());
			mPhysicsObject->SetOrientation(mTransformComponent->GetRotation());
			mPhysicsObject->Integrate(deltaTime);
		}
		else // Dynamic
		{
			mTransformComponent->SetPosition(mPhysicsObject->GetPosition());
			mTransformComponent->SetRotation(mPhysicsObject->GetOrientation());
		}
	}
}

void BaseColliderComponent::OnEnable()
{
	if(mRigidBodyComponent == nullptr)
	{
		mRigidBodyComponent = GetOwner().GetComponent<RigidBodyComponent>();
	}

	std::unique_ptr<Collider> collider = CreateCollider();
	const PhysicsObjectType objectType = mRigidBodyComponent ? (mRigidBodyComponent->IsKinematic() ? PhysicsObjectType::Kinematic : PhysicsObjectType::Dynamic) : PhysicsObjectType::Static;
	mPhysicsObject = mPhysicsService->GetPhysicsWorld().CreatePhysicsObject(std::move(collider), objectType);

	UpdatePhysicsObjectPropertys();

	if (mPhysicsObject != nullptr)
	{
		OnTriggerEnterHandle = mPhysicsObject->GetOnTriggerEnterDelegate().AddRaw(this, &BaseColliderComponent::NotifyParentOnTriggerEnter);
		OnTriggerStayHandle = mPhysicsObject->GetOnTriggerStayDelegate().AddRaw(this, &BaseColliderComponent::NotifyParentOnTriggerStay);
		OnTriggerExitHandle = mPhysicsObject->GetOnTriggerExitDelegate().AddRaw(this, &BaseColliderComponent::NotifyParentOnTriggerExit);

		if (mPhysicsObject->GetIsStatic())
		{
			//OnPositionChangedHandle = mTransformComponent->GetOnPositionChangeDelegate().AddRaw(this, &BaseColliderComponent::OnPositionChanged);
			//OnRotationChangedHandle = mTransformComponent->GetOnScaleChangeDelegate().AddRaw(this, &BaseColliderComponent::OnRotationChanged);
		}
	}
}

void BaseColliderComponent::OnDisable()
{
	if (mPhysicsObject != nullptr)
	{
		mPhysicsObject->GetOnTriggerEnterDelegate().Remove(OnTriggerEnterHandle);
		mPhysicsObject->GetOnTriggerStayDelegate().Remove(OnTriggerStayHandle);
		mPhysicsObject->GetOnTriggerExitDelegate().Remove(OnTriggerExitHandle);

		mPhysicsService->GetPhysicsWorld().RemoveObject(mPhysicsObject);
		mPhysicsObject = nullptr;
	}

	if (OnPositionChangedHandle.IsValid())
	{
		mTransformComponent->GetOnScaleChangeDelegate().Remove(OnPositionChangedHandle);
	}

	if (OnRotationChangedHandle.IsValid())
	{
		mTransformComponent->GetOnScaleChangeDelegate().Remove(OnRotationChangedHandle);
	}
}

const Vector3 BaseColliderComponent::GetCenter() const 
{ 
	if (mTransformComponent)
	{
		Vector3 rotatedOffset = mTransformComponent->GetRotation() * mCenter;
		return mTransformComponent->GetPosition() + rotatedOffset;
	}

	return mCenter; 
}

void BaseColliderComponent::SetIsTrigger(bool isTrigger)
{
	mIsTrigger = isTrigger;

	if (mPhysicsObject)
	{
		mPhysicsObject->SetIsTrigger(mIsTrigger);
	}
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

	mPhysicsObject->SetIsTrigger(mIsTrigger);
	mPhysicsObject->UpdateInverseMass();
}

void BaseColliderComponent::NotifyParentOnTriggerEnter(Collider* collider)
{
	GetOwner().OnTriggerEnter(collider);
}

void BaseColliderComponent::NotifyParentOnTriggerStay(Collider* collider)
{
	GetOwner().OnTriggerStay(collider);
}

void BaseColliderComponent::NotifyParentOnTriggerExit(Collider* collider)
{
	GetOwner().OnTriggerExit(collider);
}

void BaseColliderComponent::OnPositionChanged(const Vector3& position)
{

}

void BaseColliderComponent::OnRotationChanged(const Quaternion& rotation)
{

}
