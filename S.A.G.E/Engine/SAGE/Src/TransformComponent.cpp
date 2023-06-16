#include "Precompiled.h"
#include "TransformComponent.h"

#include "GameObject.h"
#include "RigidBodyComponent.h"

using namespace SAGE;
using namespace SAGE::Math;

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