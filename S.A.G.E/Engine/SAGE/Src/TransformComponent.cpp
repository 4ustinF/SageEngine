#include "Precompiled.h"
#include "TransformComponent.h"

#include "GameObject.h"
#include "GameObjectHandle.h"
#include "GameWorld.h"
#include "RigidBodyComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(TransformComponent, 500);

void TransformComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Transform Component##TransformComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		Vector3 pos = mTransform.position;
		if (ImGui::DragFloat3("Position##TransformComponent", &pos.x, 0.1f))
		{
			SetPosition(pos - mLocalTransform.position); // TODO: Figure out when we are calling set position on a child to update that accordingly?
		}

		if (ImGui::DragFloat3("Local Position##TransformComponent", &mLocalTransform.position.x, 0.1f))
		{
			SetLocalPosition(mLocalTransform.position);
		}

		if (ImGui::DragFloat3("Rotation##TransformComponent", &mDegreeAngles.x, 0.1f))
		{
			SetRotation(mDegreeAngles);
		}

		if (ImGui::DragFloat3("Scale##TransformComponent", &mTransform.scale.x, 0.1f))
		{
			SetScale(mTransform.scale);
		}
	}

	Graphics::SimpleDraw::AddTransform(mTransform.GetMatrix4()); // TODO: This should not grow with scale. 
}

void TransformComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	// --- Position ---
	if (mTransform.position != Vector3::Zero)
	{
		rj::Value position(rj::kArrayType);
		position.PushBack(mTransform.position.x, allocator);
		position.PushBack(mTransform.position.y, allocator);
		position.PushBack(mTransform.position.z, allocator);

		compObj.AddMember("Position", position, allocator);
	}

	// --- Rotation ---
	if (mDegreeAngles != Vector3::Zero)
	{
		rj::Value rotation(rj::kArrayType);
		rotation.PushBack(mDegreeAngles.x, allocator);
		rotation.PushBack(mDegreeAngles.y, allocator);
		rotation.PushBack(mDegreeAngles.z, allocator);

		compObj.AddMember("Rotation", rotation, allocator);
	}

	// --- Scale ---
	if (mTransform.scale != Vector3::One)
	{
		rj::Value scale(rj::kArrayType);
		scale.PushBack(mTransform.scale.x, allocator);
		scale.PushBack(mTransform.scale.y, allocator);
		scale.PushBack(mTransform.scale.z, allocator);

		compObj.AddMember("Scale", scale, allocator);
	}
}

void TransformComponent::SetPosition(const Vector3& inPos)
{
	mTransform.position = inPos + mLocalTransform.position;
	mOnPositionChange.Broadcast(mTransform.position);

	for (const GameObjectHandle& childHandle : GetOwner().GetChildrenHandles())
	{
		UpdateRecursivePosition(childHandle, mTransform.position);
	}

	// TODO: Remove this block.
	if (RigidBodyComponent* rbc = GetOwner().GetComponent<RigidBodyComponent>())
	{
		SAGE::Graphics::Transform transform;
		transform.position = mTransform.position;
		transform.rotation = mTransform.rotation;
		transform.scale = mTransform.scale;
		auto rb = rbc->GetRigidBody();
		rb->setWorldTransform(ConvertToBtTransform(transform));
	}
}

void TransformComponent::SetRotation(const SAGE::Math::Vector3& inRotation)
{
	mDegreeAngles = inRotation;
	mTransform.rotation = Quaternion::RotationEuler(mDegreeAngles * Constants::DegToRad);
}

void TransformComponent::SetRotation(const SAGE::Math::Quaternion& inRotation)
{
	mTransform.rotation = inRotation;
	mDegreeAngles = mTransform.rotation.ToClampedDegree();
}

void TransformComponent::SetScale(const SAGE::Math::Vector3& inScale)
{
	mTransform.scale = inScale;
	mOnScaleChange.Broadcast(inScale);
}

void TransformComponent::SetLocalPosition(const SAGE::Math::Vector3& inPos)
{
	mLocalTransform.position = inPos;
}

void TransformComponent::SetLocalRotation(const SAGE::Math::Vector3& inRotation)
{
	mLocalDegreeAngles = inRotation;
	mLocalTransform.rotation = Quaternion::RotationEuler(mLocalDegreeAngles * Constants::DegToRad);
}

void TransformComponent::SetLocalRotation(const SAGE::Math::Quaternion& inRotation)
{
	mLocalTransform.rotation = inRotation;
	mLocalDegreeAngles = mLocalTransform.rotation.ToClampedDegree();
}

void TransformComponent::SetLocalScale(const SAGE::Math::Vector3& inScale)
{
	mLocalTransform.scale = inScale;
}

void TransformComponent::UpdateRecursivePosition(const GameObjectHandle& gameObjectHandle, const Vector3& inWorldPos)
{
	if (GameObject* gameObject = GetOwner().GetWorld().GetGameObject(gameObjectHandle))
	{
		if (TransformComponent* transformComponent = gameObject->GetComponent<TransformComponent>())
		{
			transformComponent->SetPosition(inWorldPos);
			return;
		}
	}

	for (const GameObjectHandle& childHandle : GetOwner().GetChildrenHandles())
	{
		UpdateRecursivePosition(childHandle, inWorldPos);
	}
}
