#include "Precompiled.h"
#include "RigidBodyComponent.h"

#include "GameObject.h"
#include "GameWorld.h"
#include "TransformComponent.h"
#include "ColliderComponent.h"
#include "BPhysicsService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(RigidBodyComponent, 100);

void RigidBodyComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Rigid Body Component ##RigidBodyComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Text("Mass");
		ImGui::SameLine();
		if (ImGui::InputFloat("##RigidBodyComponent", &mMass))
		{
			SetMass(mMass);
		}

		ImGui::Separator();

		ImGui::Text("Constraints");
		ImGui::Text("Freeze Position");
		ImGui::SameLine();
		if (ImGui::Checkbox("X##RBComponentFreezePos", &mFreezePositionX))
		{
			UpdateConstraint();
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Y##RBComponentFreezePos", &mFreezePositionY))
		{
			UpdateConstraint();
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Z##RBComponentFreezePos", &mFreezePositionZ))
		{
			UpdateConstraint();
		}

		ImGui::Text("Freeze Rotation");
		ImGui::SameLine();
		if (ImGui::Checkbox("X##RBComponentFreezeRot", &mFreezeRotationX))
		{
			UpdateConstraint();
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Y##RBComponentFreezeRot", &mFreezeRotationY))
		{
			UpdateConstraint();
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Z##RBComponentFreezeRot", &mFreezeRotationZ))
		{
			UpdateConstraint();
		}
	}
}

void RigidBodyComponent::OnEnable()
{
	const auto& colliderComponent = GetOwner().GetComponent<ColliderComponent>();
	const auto& transformComponent = GetOwner().GetComponent<TransformComponent>();

	auto collisionShape = colliderComponent->GetCollisionShape();
	btVector3 localIntertia(0.0f, 0.0f, 0.0f);
	if (IsDynamic())
	{
		collisionShape->calculateLocalInertia(mMass, localIntertia);
	}

	Transform origin;
	origin.position = colliderComponent->GetCenter();

	btDefaultMotionState* motionState = new btDefaultMotionState(ConvertToBtTransform(*transformComponent), ConvertToBtTransform(origin));
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mMass, motionState, collisionShape, localIntertia);
	mRigidBody = new btRigidBody(rbInfo);

	// CollisionGroups
	mRigidBody->setUserIndex(CollisionGroups::Default);

	// Constraints
	mIsInitialized = true;
	if (mFreezePositionX || mFreezePositionY || mFreezePositionZ || mFreezeRotationX || mFreezeRotationY || mFreezeRotationZ)
	{
		UpdateConstraint();
	}

	auto physicsService = GetOwner().GetWorld().GetService<BPhysicsService>();
	physicsService->AddRigidBody(this);
}

void RigidBodyComponent::OnDisable()
{
	auto physicsService = GetOwner().GetWorld().GetService<BPhysicsService>();
	physicsService->RemoveRigidBody(this);

	if (mConstraint != nullptr)
	{
		physicsService->GetDynamicsWorld()->removeConstraint(mConstraint);
	}

	if (mRigidBody != nullptr && mRigidBody->getMotionState())
	{
		delete mRigidBody->getMotionState();
	}

	SafeDelete(mRigidBody);
	SafeDelete(mConstraint);
}

void RigidBodyComponent::SetMass(float mass)
{
	mMass = mass;

	if (mRigidBody != nullptr)
	{
		btVector3 inertia(0.0f, 0.0f, 0.0f);
		mRigidBody->getCollisionShape()->calculateLocalInertia(mMass, inertia);
		mRigidBody->setMassProps(mMass, inertia);
	}
}

bool RigidBodyComponent::IsDynamic() const
{
	return mMass > 0.0f;
}

btRigidBody* RigidBodyComponent::GetRigidBody()
{
	return mRigidBody;
}

void RigidBodyComponent::SetConstraints(const Vector3Int& pos, const Vector3Int& rot)
{
	mFreezePositionX = pos.x == 1;
	mFreezePositionY = pos.y == 1;
	mFreezePositionZ = pos.z == 1;
	mFreezeRotationX = rot.x == 1;
	mFreezeRotationY = rot.y == 1;
	mFreezeRotationZ = rot.z == 1;

	if (!mIsInitialized)
	{
		return;
	}

	if (mFreezePositionX || mFreezePositionY || mFreezePositionZ || mFreezeRotationX || mFreezeRotationY || mFreezeRotationZ)
	{
		UpdateConstraint();
	}
}

void RigidBodyComponent::AddCollisionMask(CollisionGroups collisionGroup)
{
	if (!mRigidBody)
	{
		return;
	}

	// Add a mask
	int currentMask = mRigidBody->getBroadphaseHandle()->m_collisionFilterMask;
	int newMask = currentMask | collisionGroup;
	mRigidBody->getBroadphaseHandle()->m_collisionFilterMask = newMask;
}

void RigidBodyComponent::RemoveCollisionMask(CollisionGroups collisionGroup)
{
	if (!mRigidBody)
	{
		return;
	}

	// Remove a mask
	int currentMask = mRigidBody->getBroadphaseHandle()->m_collisionFilterMask;
	int newMask = currentMask & ~collisionGroup;
	mRigidBody->getBroadphaseHandle()->m_collisionFilterMask = newMask;
}

void RigidBodyComponent::UpdateConstraint()
{
	auto dynamicsWolrd = GetOwner().GetWorld().GetService<BPhysicsService>()->GetDynamicsWorld();

	// Remove the constraint to the world
	if (mConstraint != nullptr)
	{
		dynamicsWolrd->removeConstraint(mConstraint);
	}

	//mConstraint = new btGeneric6DofSpringConstraint(*mRigidBody, btTransform::getIdentity(), false);
	const btScalar freezePosX = mFreezePositionX ? static_cast<btScalar>(0.0f) : static_cast<btScalar>(1.0f);
	const btScalar freezePosY = mFreezePositionY ? static_cast<btScalar>(0.0f) : static_cast<btScalar>(1.0f);
	const btScalar freezePosZ = mFreezePositionZ ? static_cast<btScalar>(0.0f) : static_cast<btScalar>(1.0f);
	const btScalar freezeRotX = mFreezeRotationX ? static_cast<btScalar>(0.0f) : static_cast<btScalar>(1.0f);
	const btScalar freezeRotY = mFreezeRotationY ? static_cast<btScalar>(0.0f) : static_cast<btScalar>(1.0f);
	const btScalar freezeRotZ = mFreezeRotationZ ? static_cast<btScalar>(0.0f) : static_cast<btScalar>(1.0f);

	//// Set the limits for linear motion to zero, to disable translation
	//mConstraint->setLinearLowerLimit(btVector3(freezePosX, freezePosY, freezePosZ));
	//mConstraint->setLinearUpperLimit(btVector3(-freezePosX, -freezePosY, -freezePosZ));

	//// Set the limits for Angular motion to zero, to disable rotation
	//mConstraint->setAngularLowerLimit(btVector3(freezeRotX, freezeRotY, freezeRotZ));
	//mConstraint->setAngularUpperLimit(btVector3(-freezeRotX, -freezeRotY, -freezeRotZ));

	mRigidBody->setLinearFactor(btVector3(freezePosX, freezePosY, freezePosZ));
	mRigidBody->setAngularFactor(btVector3(freezeRotX, freezeRotY, freezeRotZ));

	// Add the constraint to the world
	//dynamicsWolrd->addConstraint(mConstraint);;
}

void RigidBodyComponent::SetCollisionGroup(CollisionGroups collisionGroup)
{
	if (!mRigidBody)
	{
		return;
	}

	int currentFlags = mRigidBody->getBroadphaseHandle()->m_collisionFilterGroup;
	int newFlags = (currentFlags & ~btBroadphaseProxy::DefaultFilter) | collisionGroup;
	mRigidBody->getBroadphaseHandle()->m_collisionFilterGroup = newFlags;
}