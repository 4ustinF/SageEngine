#include "Precompiled.h"
#include "TransformComponent.h"

#include "GameObject.h"
#include "RigidBodyComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(TransformComponent, 500);

void TransformComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Transform Component##TransformComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (ImGui::DragFloat3("Position##TransformComponent", &mTransform.position.x, 0.1f))
		{
			SetPosition(mTransform.position); // TODO: Remove this is dumb. All because old physics system smh.
		}

		if (ImGui::DragFloat3("Rotation##TransformComponent", &mDegreeAngles.x, 0.1f))
		{
			SetRotation(mDegreeAngles);
		}

		ImGui::DragFloat3("Scale##TransformComponent", &mTransform.scale.x, 0.1f);
	}

	Graphics::SimpleDraw::AddTransform(mTransform.GetMatrix4());
}

void TransformComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	// --- Position ---
	{
		rj::Value position(rj::kArrayType);
		position.PushBack(mTransform.position.x, allocator);
		position.PushBack(mTransform.position.y, allocator);
		position.PushBack(mTransform.position.z, allocator);

		compObj.AddMember("Position", position, allocator);
	}

	// --- Rotation ---
	{
		rj::Value rotation(rj::kArrayType);
		rotation.PushBack(mDegreeAngles.x, allocator);
		rotation.PushBack(mDegreeAngles.y, allocator);
		rotation.PushBack(mDegreeAngles.z, allocator);

		compObj.AddMember("Rotation", rotation, allocator);
	}

	// --- Scale ---
	{
		rj::Value scale(rj::kArrayType);
		scale.PushBack(mTransform.scale.x, allocator);
		scale.PushBack(mTransform.scale.y, allocator);
		scale.PushBack(mTransform.scale.z, allocator);

		compObj.AddMember("Scale", scale, allocator);
	}
}

void TransformComponent::SetPosition(const Vector3& InPos)
{
	mTransform.position = InPos;

	// TODO: Remove this block.
	auto rbc = GetOwner().GetComponent<RigidBodyComponent>();
	if (rbc != nullptr)
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
}
