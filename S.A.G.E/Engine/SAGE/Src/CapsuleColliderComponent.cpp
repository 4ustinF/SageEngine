#include "Precompiled.h"
#include "CapsuleColliderComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(CapsuleColliderComponent, 200);

void CapsuleColliderComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	BaseColliderComponent::LoadComponentFromTemplate(value);

	if (value.HasMember("Radius"))
	{
		SetRadius(value["Radius"].GetFloat());
	}

	if (value.HasMember("Height"))
	{
		SetRadius(value["Height"].GetFloat());
	}
}

void CapsuleColliderComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	BaseColliderComponent::SaveComponentToTemplate(compObj, allocator);
	SaveNumberToTemplate(compObj, allocator, "Radius", mRadius, 0.5f);
	SaveNumberToTemplate(compObj, allocator, "Height", mHeight, 1.0f);
}

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
		ImGui::Checkbox("Debug Fill##BoxColliderComponent", &mDebugFill);
		ImGui::Checkbox("Is Trigger##CapsuleColliderComponent ", &mIsTrigger);
		ImGui::DragFloat3("Center##CapsuleColliderComponent ", &mCenter.x, 0.1f);
		if (ImGui::DragFloat("Radius##BoxColliderComponent", &mRadius, 0.1f)) { SetRadius(mRadius); }
		if (ImGui::DragFloat("Height##BoxColliderComponent", &mHeight, 0.1f)) { SetHeight(mHeight); }
	}

	if (mDebugFill) // TODO: 
	{
		SimpleDraw::AddCylinder(GetCenter(), 16, 16, mRadius, mHeight, mDebugColor, true);
	}
	else
	{
		SimpleDraw::AddCylinder(GetCenter(), 16, 16, mRadius, mHeight, mDebugColor, true);
	}
}

void CapsuleColliderComponent::SetRadius(float radius) 
{ 
	mRadius = Max(0.1f, radius);
}

void CapsuleColliderComponent::SetHeight(float height) 
{ 
	mHeight = Max(0.1f, height);
}
