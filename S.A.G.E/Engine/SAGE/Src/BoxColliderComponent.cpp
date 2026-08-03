#include "Precompiled.h"
#include "BoxColliderComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;

MEMORY_POOL_DEFINE(BoxColliderComponent , 200);

void BoxColliderComponent ::Initialize()
{
	BaseColliderComponent::Initialize();
}

void BoxColliderComponent ::Terminate()
{
	BaseColliderComponent::Terminate();
}

void BoxColliderComponent ::DebugUI()
{
	if (ImGui::CollapsingHeader("Box Collider Component##BoxColliderComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Checkbox("Is Trigger##BoxColliderComponent", &mIsTrigger);
		ImGui::DragFloat3("Center##BoxColliderComponent", &mCenter.x, 0.1f);
		if (ImGui::DragFloat3("Size##BoxColliderComponent", &mSize.x, 0.1f)) { SetSize(mSize); }
	}

	//Graphics::SimpleDraw::AddAABB(aabb.center, aabb.extend, Graphics::Colors::Green);
}

void BoxColliderComponent::SetSize(const Math::Vector3& size)
{ 
	mSize.x = Max(0.1f, size.x);
	mSize.y = Max(0.1f, size.y);
	mSize.z = Max(0.1f, size.z);
}