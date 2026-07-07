#include "Precompiled.h"
#include "TransformComponent.h"

#include "GameObject.h"
#include "GameObjectHandle.h"
#include "GameWorld.h"
#include "RigidBodyComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(TransformComponent, 300);

void TransformComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	const bool hasParentTransformComponent = FindParentTransformComponent() != nullptr;

	if (value.HasMember("Position"))
	{
		const auto& position = value["Position"].GetArray();
		const float x = position[0].GetFloat();
		const float y = position[1].GetFloat();
		const float z = position[2].GetFloat();
		hasParentTransformComponent ? SetLocalPosition(Vector3(x, y, z)) : SetPosition(Vector3(x, y, z));
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
		hasParentTransformComponent ? SetLocalRotation(Vector3(x, y, z)) : SetRotation(Vector3(x, y, z));
	}

	if (value.HasMember("Local Rotation"))
	{
		const auto& rotation = value["Local Rotation"].GetArray();
		const float x = rotation[0].GetFloat();
		const float y = rotation[1].GetFloat();
		const float z = rotation[2].GetFloat();
		SetLocalRotation(Vector3(x, y, z));
	}

	if (value.HasMember("Scale"))
	{
		const auto& scale = value["Scale"].GetArray();
		const float x = scale[0].GetFloat();
		const float y = scale[1].GetFloat();
		const float z = scale[2].GetFloat();
		hasParentTransformComponent ? SetLocalScale(Vector3(x, y, z)) : SetScale(Vector3(x, y, z));
	}

	if (value.HasMember("Local Scale"))
	{
		const auto& scale = value["Local Scale"].GetArray();
		const float x = scale[0].GetFloat();
		const float y = scale[1].GetFloat();
		const float z = scale[2].GetFloat();
		SetLocalScale(Vector3(x, y, z));
	}
}

void TransformComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	const bool hasParentTransformComponent = FindParentTransformComponent() != nullptr;

	// --- Position ---
	if (mTransform.position != Vector3::Zero && !hasParentTransformComponent)
	{
		rj::Value position(rj::kArrayType);
		position.PushBack(mTransform.position.x, allocator);
		position.PushBack(mTransform.position.y, allocator);
		position.PushBack(mTransform.position.z, allocator);

		compObj.AddMember("Position", position, allocator);
	}

	// --- Local Position ---
	if (mLocalTransform.position != Vector3::Zero)
	{
		rj::Value localPosition(rj::kArrayType);
		localPosition.PushBack(mLocalTransform.position.x, allocator);
		localPosition.PushBack(mLocalTransform.position.y, allocator);
		localPosition.PushBack(mLocalTransform.position.z, allocator);

		compObj.AddMember("Local Position", localPosition, allocator);
	}

	// --- Rotation ---
	if (mDegreeAngles != Vector3::Zero && !hasParentTransformComponent)
	{
		rj::Value rotation(rj::kArrayType);
		rotation.PushBack(mDegreeAngles.x, allocator);
		rotation.PushBack(mDegreeAngles.y, allocator);
		rotation.PushBack(mDegreeAngles.z, allocator);

		compObj.AddMember("Rotation", rotation, allocator);
	}

	// --- Local Rotation ---
	if (mLocalDegreeAngles != Vector3::Zero)
	{
		rj::Value localRotation(rj::kArrayType);
		localRotation.PushBack(mLocalDegreeAngles.x, allocator);
		localRotation.PushBack(mLocalDegreeAngles.y, allocator);
		localRotation.PushBack(mLocalDegreeAngles.z, allocator);

		compObj.AddMember("Local Rotation", localRotation, allocator);
	}

	// --- Scale ---
	if (mTransform.scale != Vector3::One && !hasParentTransformComponent)
	{
		rj::Value scale(rj::kArrayType);
		scale.PushBack(mTransform.scale.x, allocator);
		scale.PushBack(mTransform.scale.y, allocator);
		scale.PushBack(mTransform.scale.z, allocator);

		compObj.AddMember("Scale", scale, allocator);
	}

	// --- Local Scale ---
	if (mLocalTransform.scale != Vector3::One)
	{
		rj::Value localScale(rj::kArrayType);
		localScale.PushBack(mLocalTransform.scale.x, allocator);
		localScale.PushBack(mLocalTransform.scale.y, allocator);
		localScale.PushBack(mLocalTransform.scale.z, allocator);

		compObj.AddMember("Local Scale", localScale, allocator);
	}
}

void TransformComponent::TransformComponent::Initialize()
{
	if (const TransformComponent* transformComponent = FindParentTransformComponent())
	{
		const Transform& parentTransform = transformComponent->GetTransform();
		mTransform.position = parentTransform.position + mLocalTransform.position; // parent.pos + (parent.rot * (parent.scale * local.pos)); // Scale and rotation might effect pos?
		mTransform.rotation = parentTransform.rotation * mLocalTransform.rotation;
		mTransform.scale = parentTransform.scale * mLocalTransform.scale;
	}

	mDegreeAngles = mTransform.rotation.ToClampedDegree();
	mLocalDegreeAngles = mLocalTransform.rotation.ToClampedDegree();
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

		Vector3 rot = mDegreeAngles;
		if (ImGui::DragFloat3("Rotation##TransformComponent", &rot.x, 0.1f))
		{
			SetRotation(rot);
		}

		Vector3 locRot = mLocalDegreeAngles;
		if (ImGui::DragFloat3("Local Rotation##TransformComponent", &locRot.x, 0.1f))
		{
			SetLocalRotation(locRot);
		}

		Vector3 scale = mTransform.scale;
		if (ImGui::DragFloat3("Scale##TransformComponent", &scale.x, 0.1f))
		{
			SetScale(scale);
		}

		Vector3 localScale = mLocalTransform.scale;
		if (ImGui::DragFloat3("Local Scale##TransformComponent", &localScale.x, 0.1f))
		{
			SetLocalScale(localScale);
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

void TransformComponent::SetRotation(const Vector3& inRotation)
{
	SetRotation(Quaternion::RotationEuler(inRotation * Constants::DegToRad));
}

void TransformComponent::SetRotation(const Quaternion& inRotation)
{
	if (const TransformComponent* parent = FindParentTransformComponent())
	{
		// Convert world -> local
		mLocalTransform.rotation = parent->GetTransform().rotation * Conjugate(inRotation); // TODO?
		//mLocalTransform.rotation = parent->GetTransform().rotation * inRotation;
	}
	else
	{
		mLocalTransform.rotation = inRotation;
	}

	mLocalDegreeAngles = mLocalTransform.rotation.ToClampedDegree();
	UpdateWorldRotation(inRotation);
}

void TransformComponent::SetScale(const Vector3& inScale)
{
	if (const TransformComponent* parent = FindParentTransformComponent())
	{
		// Convert world -> local
		mLocalTransform.scale = inScale - parent->GetTransform().scale;
	}
	else
	{
		mLocalTransform.scale = inScale;
	}

	UpdateWorldScale(inScale);
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

void TransformComponent::SetLocalRotation(const Vector3& inRotation)
{
	SetLocalRotation(Quaternion::RotationEuler(inRotation * Constants::DegToRad));
}

void TransformComponent::SetLocalRotation(const Quaternion& inRotation)
{
	mLocalTransform.rotation = inRotation;
	mLocalDegreeAngles = mLocalTransform.rotation.ToClampedDegree();

	if (const TransformComponent* parentTransformComp = FindParentTransformComponent())
	{
		UpdateWorldRotation(parentTransformComp->GetTransform().rotation * mLocalTransform.rotation);
	}
	else
	{
		UpdateWorldRotation(mLocalTransform.rotation);
	}
}

void TransformComponent::SetLocalScale(const Vector3& inScale)
{
	mLocalTransform.scale = inScale;

	if (const TransformComponent* parent = FindParentTransformComponent())
	{
		UpdateWorldScale(parent->GetTransform().scale * mLocalTransform.scale);
	}
	else
	{
		UpdateWorldScale(mLocalTransform.scale);
	}
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

void TransformComponent::UpdateWorldPosition(const Vector3& inPos)
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

	// If this object has a transform comp → update it
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

void TransformComponent::UpdateWorldRotation(const Quaternion& inRotation)
{
	mTransform.rotation = inRotation;
	mDegreeAngles = mTransform.rotation.ToClampedDegree();
	mOnRotationChange.Broadcast(inRotation);

	for (const GameObjectHandle& childHandle : GetOwner().GetChildrenHandles())
	{
		UpdateChildrenRotation(childHandle, inRotation);
	}
}

void TransformComponent::UpdateChildrenRotation(const GameObjectHandle& gameObjectHandle, const Quaternion& inWorldRotation)
{
	GameObject* gameObject = GetOwner().GetWorld().GetGameObject(gameObjectHandle);
	if (gameObject == nullptr)
	{
		return;
	}

	// If this object has a transform comp → update it
	if (TransformComponent* transformComponent = gameObject->GetComponent<TransformComponent>())
	{
		transformComponent->SetLocalRotation(transformComponent->mLocalTransform.rotation);

		// Recurse this object's children using this objects world rotation.
		for (const GameObjectHandle& childHandle : gameObject->GetChildrenHandles())
		{
			transformComponent->UpdateChildrenRotation(childHandle, transformComponent->mTransform.rotation);
		}
	}
	else
	{
		// Recurse this object's children using this the passed in world rotation.
		for (const GameObjectHandle& childHandle : gameObject->GetChildrenHandles())
		{
			UpdateChildrenRotation(childHandle, inWorldRotation);
		}
	}
}

void TransformComponent::UpdateWorldScale(const Vector3& inScale)
{
	mTransform.scale = inScale;
	mOnScaleChange.Broadcast(inScale);

	for (const GameObjectHandle& childHandle : GetOwner().GetChildrenHandles())
	{
		UpdateChildrenScales(childHandle, inScale);
	}
}

void TransformComponent::UpdateChildrenScales(const GameObjectHandle& gameObjectHandle, const SAGE::Math::Vector3& inWorldScale)
{
	GameObject* gameObject = GetOwner().GetWorld().GetGameObject(gameObjectHandle);
	if (gameObject == nullptr)
	{
		return;
	}

	// If this object has a transform comp → update it
	if (TransformComponent* transformComponent = gameObject->GetComponent<TransformComponent>())
	{
		transformComponent->SetLocalScale(transformComponent->mLocalTransform.scale);

		// Recurse this object's children using this objects world scale.
		for (const GameObjectHandle& childHandle : gameObject->GetChildrenHandles())
		{
			transformComponent->UpdateChildrenScales(childHandle, transformComponent->mTransform.scale);
		}
	}
	else
	{
		// Recurse this object's children using this the passed in world scale.
		for (const GameObjectHandle& childHandle : gameObject->GetChildrenHandles())
		{
			UpdateChildrenScales(childHandle, inWorldScale);
		}
	}
}