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

void TransformComponent::LoadComponentFromTemplate(const rapidjson::Value& value)
{
	if (value.HasMember("Position"))
	{
		const auto& position = value["Position"].GetArray();
		const float x = position[0].GetFloat();
		const float y = position[1].GetFloat();
		const float z = position[2].GetFloat();
		SetPosition(Vector3(x, y, z));
	}

	if (value.HasMember("Local Position"))
	{
		const auto& position = value["Local Position"].GetArray();
		const float x = position[0].GetFloat();
		const float y = position[1].GetFloat();
		const float z = position[2].GetFloat();
		SetLocalPosition(Vector3(x, y, z));
	}

	if (value.HasMember("Rotation"))
	{
		const auto& rotation = value["Rotation"].GetArray();
		const float x = rotation[0].GetFloat();
		const float y = rotation[1].GetFloat();
		const float z = rotation[2].GetFloat();
		SetRotation(Vector3(x, y, z));
	}

	if (value.HasMember("Scale"))
	{
		const auto& scale = value["Scale"].GetArray();
		const float x = scale[0].GetFloat();
		const float y = scale[1].GetFloat();
		const float z = scale[2].GetFloat();
		SetScale(Vector3(x, y, z));
	}
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

	// --- Position ---
	if (mLocalTransform.position != Vector3::Zero)
	{
		rj::Value localPosition(rj::kArrayType);
		localPosition.PushBack(mLocalTransform.position.x, allocator);
		localPosition.PushBack(mLocalTransform.position.y, allocator);
		localPosition.PushBack(mLocalTransform.position.z, allocator);

		compObj.AddMember("Local Position", localPosition, allocator);
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

void TransformComponent::TransformComponent::Initialize()
{
	if (const TransformComponent* transformComponent = FindParentTransformComponent())
	{
		mTransform.position = transformComponent->GetTransform().position + mLocalTransform.position;
	}
}

void TransformComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Transform Component##TransformComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		Vector3 pos = mTransform.position;
		if (ImGui::DragFloat3("Position##TransformComponent", &pos.x, 0.1f))
		{
			SetPosition(pos);
		}

		Vector3 locPos = mLocalTransform.position;
		if (ImGui::DragFloat3("Local Position##TransformComponent", &locPos.x, 0.1f))
		{
			SetLocalPosition(locPos);
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

void TransformComponent::SetPosition(const Vector3& inPos)
{
	if (const TransformComponent* parent = FindParentTransformComponent())
	{
		// Convert world -> local
		mLocalTransform.position = inPos - parent->GetTransform().position;
	}
	else
	{
		mLocalTransform.position = inPos;
	}

	UpdateWorldPosition(inPos);

	// TODO: Remove this block.
	if (RigidBodyComponent* rbc = GetOwner().GetComponent<RigidBodyComponent>()) 
	{
		SAGE::Graphics::Transform transform;
		transform.position = mTransform.position;
		transform.rotation = mTransform.rotation;
		transform.scale = mTransform.scale;

		if (auto rb = rbc->GetRigidBody())
		{
			rb->setWorldTransform(ConvertToBtTransform(transform));
		}
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

void TransformComponent::SetLocalPosition(const Vector3& inPos)
{
	mLocalTransform.position = inPos;

	if (const TransformComponent* parent = FindParentTransformComponent())
	{
		UpdateWorldPosition(parent->GetTransform().position + mLocalTransform.position);
	}
	else
	{
		UpdateWorldPosition(mLocalTransform.position);
	}
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

const TransformComponent* TransformComponent::FindParentTransformComponent() const
{
	GameObject* current = GetOwner().GetParentGameObject();
	while (current != nullptr)
	{
		if (const TransformComponent* transform = current->GetComponent<TransformComponent>())
		{
			return transform;
		}

		current = current->GetParentGameObject();
	}

	return nullptr; // nothing found
}

void TransformComponent::UpdateWorldPosition(const SAGE::Math::Vector3& inPos)
{
	mTransform.position = inPos;
	mOnPositionChange.Broadcast(inPos);

	for (const GameObjectHandle& childHandle : GetOwner().GetChildrenHandles())
	{
		UpdateChildrenPositions(childHandle, inPos);
	}
}

void TransformComponent::UpdateChildrenPositions(const GameObjectHandle& gameObjectHandle, const Vector3& inWorldPos)
{
	GameObject* gameObject = GetOwner().GetWorld().GetGameObject(gameObjectHandle);
	if (gameObject == nullptr)
	{
		return;
	}

	// If this object has a transform → update it
	if (TransformComponent* transformComponent = gameObject->GetComponent<TransformComponent>())
	{
		transformComponent->SetLocalPosition(transformComponent->mLocalTransform.position);

		// Recurse this object's children using this objects world pos.
		for (const GameObjectHandle& childHandle : gameObject->GetChildrenHandles())
		{
			transformComponent->UpdateChildrenPositions(childHandle, transformComponent->mTransform.position);
		}
	}
	else
	{
		// Recurse this object's children using this the passed in world pos.
		for (const GameObjectHandle& childHandle : gameObject->GetChildrenHandles())
		{
			UpdateChildrenPositions(childHandle, inWorldPos);
		}
	}
}
