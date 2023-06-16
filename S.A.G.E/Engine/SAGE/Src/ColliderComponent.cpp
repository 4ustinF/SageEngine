#include "Precompiled.h"
#include "ColliderComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;

MEMORY_POOL_DEFINE(ColliderComponent, 200);

void ColliderComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();

	switch (mColliderShapes)
	{
	case SAGE::ColliderShapes::Box:
		mCollisionShape = new btBoxShape(btVector3(mExtend.x, mExtend.y, mExtend.z));
		break;
	case SAGE::ColliderShapes::Sphere:
		mCollisionShape = new btSphereShape(mExtend.x); // Radius
		break;
	case SAGE::ColliderShapes::Capsule:
		mCollisionShape = new btCapsuleShape(mExtend.x, mExtend.y); // Radius height
		break;
	case SAGE::ColliderShapes::StaticPlane:
		mCollisionShape = new btStaticPlaneShape(btVector3(mExtend.x, mExtend.y, mExtend.z), 1.0f); // planeNormal, planeConstant
		break;
	}
}

void ColliderComponent::Terminate()
{
	mTransformComponent = nullptr;
	SafeDelete(mCollisionShape);
}

void ColliderComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Collider Component ##ColliderComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		// TODO: Update collider on the fly
	}

	const auto& aabb = GetWorldAABB();

	switch (mColliderShapes)
	{
	case SAGE::ColliderShapes::Box:
		Graphics::SimpleDraw::AddAABB(aabb.center, aabb.extend, Graphics::Colors::Green);
		break;
	case SAGE::ColliderShapes::Sphere:
		Graphics::SimpleDraw::AddSphere(aabb.center, 10, 10, aabb.extend.x, Graphics::Colors::Green);
		break;
	case SAGE::ColliderShapes::Capsule:
		Graphics::SimpleDraw::AddCylinder(aabb.center, 10, 10, aabb.extend.x, aabb.extend.y * 2.0f, Graphics::Colors::Green);
		break;
	case SAGE::ColliderShapes::StaticPlane:
		Graphics::SimpleDraw::AddAABB(aabb.center, aabb.extend, Graphics::Colors::Green);
		//Graphics::SimpleDraw::AddPlane(20, Graphics::Colors::Green);
		break;
	}
}

Math::AABB ColliderComponent::GetLocalAABB() const
{
	return AABB(Vector3::Zero + mCenter, mExtend);
}

Math::AABB ColliderComponent::GetWorldAABB() const
{
	return AABB(mTransformComponent->position + mCenter, mExtend);
}

btCollisionShape* ColliderComponent::GetCollisionShape()
{
	return mCollisionShape;
}