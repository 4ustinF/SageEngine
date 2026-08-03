#include "Precompiled.h"
#include "CapsuleColliderComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;

MEMORY_POOL_DEFINE(CapsuleColliderComponent, 200);

void CapsuleColliderComponent ::Initialize()
{
	BaseColliderComponent::Initialize();
}

void CapsuleColliderComponent ::Terminate()
{
	BaseColliderComponent::Terminate();
}

void CapsuleColliderComponent ::DebugUI()
{
	if (ImGui::CollapsingHeader("Capsule Collider Component##CapsuleColliderComponent ", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Checkbox("Is Trigger##CapsuleColliderComponent ", &mIsTrigger);
		ImGui::DragFloat3("Center##CapsuleColliderComponent ", &mCenter.x, 0.1f);
		if (ImGui::DragFloat("Radius##BoxColliderComponent", &mRadius, 0.1f)) { SetRadius(mRadius); }
		if (ImGui::DragFloat("Height##BoxColliderComponent", &mHeight, 0.1f)) { SetHeight(mHeight); }
	}

	//Graphics::SimpleDraw::AddAABB(aabb.center, aabb.extend, Graphics::Colors::Green);
}


void CapsuleColliderComponent::SetRadius(float radius) 
{ 
	mRadius = Max(0.1f, radius);
}

void CapsuleColliderComponent::SetHeight(float height) 
{ 
	mHeight = Max(0.1f, height);
}
