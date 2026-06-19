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
		if (ImGui::DragFloat3("Position##TransformComponent", &position.x, 0.1f))
		{
			SetPosition(position);
		}
		
		ImGui::DragFloat3("Rotation##TransformComponent", &rotation.x, 0.1f);
		ImGui::DragFloat3("Scale##TransformComponent", &scale.x, 0.1f);
	}
	Graphics::SimpleDraw::AddTransform(GetMatrix4());
}

void TransformComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	// --- Position ---
	{
		rj::Value position(rj::kArrayType);
		position.PushBack(this->position.x, allocator);
		position.PushBack(this->position.y, allocator);
		position.PushBack(this->position.z, allocator);

		compObj.AddMember("Position", position, allocator);
	}

	// --- Rotation (convert back to degrees!) ---
	{
		const Vector3 euler = this->rotation.ToEuler(); // TODO: Get a better ToEuler, float precision, its slightly off by a smidge.
		rj::Value rotation(rj::kArrayType);
		rotation.PushBack(euler.x * Constants::RadToDeg, allocator);
		rotation.PushBack(euler.y * Constants::RadToDeg, allocator);
		rotation.PushBack(euler.z * Constants::RadToDeg, allocator);

		compObj.AddMember("Rotation", rotation, allocator);
	}

	// --- Scale ---
	{
		rj::Value scale(rj::kArrayType);
		scale.PushBack(this->scale.x, allocator);
		scale.PushBack(this->scale.y, allocator);
		scale.PushBack(this->scale.z, allocator);

		compObj.AddMember("Scale", scale, allocator);
	}
}


void TransformComponent::SetPosition(const Vector3& pos)
{
	position = pos;

	auto rbc = GetOwner().GetComponent<RigidBodyComponent>();
	if (rbc != nullptr)
	{
		Transform transform;
		transform.position = position;
		transform.rotation = rotation;
		transform.scale = scale;
		auto rb = rbc->GetRigidBody();
		rb->setWorldTransform(ConvertToBtTransform(transform));
	}
}