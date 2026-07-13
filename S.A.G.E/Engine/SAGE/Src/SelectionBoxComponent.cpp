#include "Precompiled.h"
#include "SelectionBoxComponent.h"

#include "GameObject.h"
#include "GameObjectHandle.h"
#include "GameWorld.h"
#include "MeshFilterComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(SelectionBoxComponent, 100);

void SelectionBoxComponent::Initialize()
{

}

void SelectionBoxComponent::Terminate()
{

}

void SelectionBoxComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Selection Box Component##SelectionBoxComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
	}

	//if (mEnableWireframe)
	if (true)
	{
		SimpleDraw::AddOBB(GetGlobalBoundingBox(), Colors::Blue); // TODO: Lets not call this every frame.
	}
}

const OBB SelectionBoxComponent::GetGlobalBoundingBox()
{
	if (mHasConstructedBoundingBox == false)
	{
		GenerateGlobalBoundingBox();
	}

	return mBoundingBox;
};

void SelectionBoxComponent::GenerateGlobalBoundingBox()
{
	// TODO: This can be outdated if we add/remove children or if the transform of this or them change.
	// TODO: Maybe for now this can be calculated every time? 
	// TODO: A static marker would be a great thing for this and other things. Tell things like children and what not to move from place.
	std::vector<Vector3> points;
	GatherOBBCorners(&GetOwner(), points);

	// TODO: Safety check size before going further.
	if (points.size() == 0)
	{
		mBoundingBox = OBB();
		return;
	}

	Vector3 min = points[0];
	Vector3 max = min;

	for (const Vector3& p : points)
	{
		min = Vector3(Min(min.x, p.x), Min(min.y, p.y), Min(min.z, p.z));
		max = Vector3(Max(max.x, p.x), Max(max.y, p.y), Max(max.z, p.z));
	}

	mBoundingBox.center = (min + max) * 0.5f;
	mBoundingBox.extend = (max - min) * 0.5f;
	mBoundingBox.extend += Vector3(Constants::Epsilon);
}

void SelectionBoxComponent::GatherOBBCorners(const GameObject* gameObject, std::vector<Vector3>& points)
{
	if (const MeshFilterComponent* meshFilterComponent = gameObject->GetComponent<MeshFilterComponent>())
	{
		const OBB& obb = meshFilterComponent->GetGlobalBoundingBox(); // TODO: Create a Gather OBB Corners func.
		points.push_back(obb.center + Vector3(obb.extend.x, obb.extend.y, obb.extend.z));
		points.push_back(obb.center + Vector3(obb.extend.x, -obb.extend.y, obb.extend.z));
		points.push_back(obb.center + Vector3(-obb.extend.x, obb.extend.y, obb.extend.z));
		points.push_back(obb.center + Vector3(-obb.extend.x, -obb.extend.y, obb.extend.z));
		points.push_back(obb.center + Vector3(obb.extend.x, obb.extend.y, -obb.extend.z));
		points.push_back(obb.center + Vector3(obb.extend.x, -obb.extend.y, -obb.extend.z));
		points.push_back(obb.center + Vector3(-obb.extend.x, obb.extend.y, -obb.extend.z));
		points.push_back(obb.center + Vector3(-obb.extend.x, -obb.extend.y, -obb.extend.z));
	}
	// TODO: Support other cases to get bounding shapes from for edit selection. 

	const GameWorld& gameWorld = GetOwner().GetWorld();
	for (const GameObjectHandle& childHandle : gameObject->GetChildrenHandles())
	{
		if (const GameObject* childGO = gameWorld.GetGameObject(childHandle))
		{
			GatherOBBCorners(childGO, points);
		}
	}
}