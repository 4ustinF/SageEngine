#include "Precompiled.h"
#include "BoxColliderComponent.h"

#include "GameWorld.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "MeshFilterComponent.h"
#include "SelectionBoxComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(BoxColliderComponent , 200);

void BoxColliderComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	BaseColliderComponent::LoadComponentFromTemplate(value);

	if (value.HasMember("Size"))
	{
		const auto& size = value["Size"].GetArray();
		const float x = size[0].GetFloat();
		const float y = size[1].GetFloat();
		const float z = size[2].GetFloat();
		SetSize(Vector3(x, y, z));
	}
}

void BoxColliderComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	BaseColliderComponent::SaveComponentToTemplate(compObj, allocator);
	SaveVector3ToTemplate(compObj, allocator, "Size", mSize, Vector3::One);
}

void BoxColliderComponent ::Initialize()
{
	BaseColliderComponent::Initialize();
	mSelectionBoxComponent = GetOwner().GetComponent<SelectionBoxComponent>();
}

void BoxColliderComponent::Terminate()
{
	BaseColliderComponent::Terminate();
}

void BoxColliderComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Box Collider Component##BoxColliderComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Checkbox("Debug Fill##BoxColliderComponent", &mDebugFill);
		ImGui::Checkbox("Is Trigger##BoxColliderComponent", &mIsTrigger);
		ImGui::DragFloat3("Center##BoxColliderComponent", &mCenter.x, 0.1f);
		if (ImGui::DragFloat3("Size##BoxColliderComponent", &mSize.x, 0.1f)) { SetSize(mSize); }
		if ((mMeshFilterComponent != nullptr || mSelectionBoxComponent != nullptr) && ImGui::Button("Resize to Mesh##BoxColliderComponent")) { ResizeToMesh(); }
	}

	const Quaternion& rotation = mPhysicsObject ? mPhysicsObject->GetOrientation() : mTransformComponent->GetRotation();
	if (mDebugFill) // TODO: Use physics object data instead.
	{
		SimpleDraw::AddFilledOBB(GetCenter(), mExtend, rotation, mDebugColor);
	}
	else
	{
		SimpleDraw::AddOBB(GetCenter(), mExtend, rotation, mDebugColor);
	}
}

void BoxColliderComponent::SetSize(const Math::Vector3& size)
{ 
	mSize.x = Max(0.1f, size.x);
	mSize.y = Max(0.1f, size.y);
	mSize.z = Max(0.1f, size.z);

	mExtend = mSize * 0.5f;
}

std::unique_ptr<Collider> BoxColliderComponent::CreateCollider()
{
	return std::make_unique<BoundingBox>(GetCenter(), mExtend, mTransformComponent->GetRotation());
}

void BoxColliderComponent::ResizeToMesh()
{
	if (mMeshFilterComponent != nullptr)
	{
		const OBB BoundingBox = mMeshFilterComponent->GetGlobalBoundingBox();
		SetSize(BoundingBox.extend * 2.0f);
		if (mTransformComponent != nullptr)
		{
			mCenter = BoundingBox.center - mTransformComponent->GetPosition();
		}
		else
		{
			mCenter = BoundingBox.center;
		}
	}
	else if (mSelectionBoxComponent != nullptr)
	{
		const OBB BoundingBox = mSelectionBoxComponent->GetGlobalBoundingBox();
		SetSize(BoundingBox.extend * 2.0f);
		if (mTransformComponent != nullptr)
		{
			mCenter = BoundingBox.center - mTransformComponent->GetPosition();
		}
		else
		{
			mCenter = BoundingBox.center;
		}
	}
}