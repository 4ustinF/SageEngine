#include "Precompiled.h"
#include "BPhysicsService.h"
#include "TransformComponent.h"
#include "BPhysicsComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "PhysicsDebugDraw.h"

#include "GameObject.h"
#include "GameWorld.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;

void BPhysicsService::Initialize()
{
	SetServiceName("BPhysics Service");

	mCollisionConfiguration = new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	mInterface = new btDbvtBroadphase();
	mSolver = new btSequentialImpulseConstraintSolver;
	mDynamicWorld = new btDiscreteDynamicsWorld(mDispatcher, mInterface, mSolver, mCollisionConfiguration);

	mDynamicWorld->setGravity(btVector3(0.0f, mGravity, 0.0f));
	mPhysicsDebugDraw = new PhysicsDebugDraw();
	mDynamicWorld->setDebugDrawer(mPhysicsDebugDraw);
	mPhysicsDebugDraw->SetGameWorld(&GetWorld());
}

void BPhysicsService::Terminate()
{
	SafeDelete(mPhysicsDebugDraw);
	SafeDelete(mDynamicWorld);
	SafeDelete(mSolver);
	SafeDelete(mInterface);
	SafeDelete(mDispatcher);
	SafeDelete(mCollisionConfiguration);
}

void BPhysicsService::Update(float deltaTime)
{
	mDynamicWorld->stepSimulation(deltaTime, mMaxSubSteps); 
	for (auto& entry : mPhysicsEntries)
	{
		entry.bPhysicsComponent->UpdatePhysics(deltaTime);
	}
}

void BPhysicsService::DebugUI()
{
	ImGui::Checkbox("Render Physics##BulletPhysics", &mRenderDebugUI);

	if (mRenderDebugUI)
	{
		int debugMode = mPhysicsDebugDraw->getDebugMode();
		bool isEnabled = (debugMode & btIDebugDraw::DBG_DrawWireframe) > 0;

		if (ImGui::Checkbox("[DBG] Draw Wire Frame##BulletPhysics", &isEnabled))
		{
			debugMode = (isEnabled) ? debugMode | btIDebugDraw::DBG_DrawWireframe : debugMode & ~btIDebugDraw::DBG_DrawWireframe;
		}

		isEnabled = (debugMode & btIDebugDraw::DBG_DrawAabb) > 0;

		if (ImGui::Checkbox("[DBG] Draw AABB##BulletPhysics", &isEnabled))
		{
			debugMode = (isEnabled) ? debugMode | btIDebugDraw::DBG_DrawAabb : debugMode & ~btIDebugDraw::DBG_DrawAabb;
		}

		isEnabled = (debugMode & btIDebugDraw::DBG_DrawNormals) > 0;

		if (ImGui::Checkbox("[DBG] Draw Normals##BulletPhysics", &isEnabled))
		{
			debugMode = (isEnabled) ? debugMode | btIDebugDraw::DBG_DrawNormals : debugMode & ~btIDebugDraw::DBG_DrawNormals;
		}

		mPhysicsDebugDraw->setDebugMode(debugMode);
		mDynamicWorld->debugDrawWorld();
		mPhysicsDebugDraw->Render();
	}
	
	//if (mRenderDebugUI)
	//{
	//	for (auto& entry : mPhysicsEntries)
	//	{
	//		entry.bPhysicsComponent->DebugUI();
	//		entry.colliderComponent->DebugUI();
	//		entry.transformComponent->DebugUI();
	//	}
	//}
}

void BPhysicsService::SetGravity(float gravity)
{
	mGravity = gravity;
	if (mDynamicWorld != nullptr)
	{
		mDynamicWorld->setGravity(btVector3(0.0f, mGravity, 0.0f));
	}
}

void BPhysicsService::Register(BPhysicsComponent* physicsComponent)
{
	Entry& entry = mPhysicsEntries.emplace_back();
	auto& gameObject = physicsComponent->GetOwner();
	entry.bPhysicsComponent = physicsComponent;
	entry.transformComponent = gameObject.GetComponent<TransformComponent>();
	entry.colliderComponent = gameObject.GetComponent<ColliderComponent>();
	entry.rigidBodyComponent = gameObject.GetComponent<RigidBodyComponent>();
}

void BPhysicsService::Unregister(BPhysicsComponent* physicsComponent)
{
	auto iter = std::find_if(mPhysicsEntries.begin(), mPhysicsEntries.end(),
		[&](const Entry& entry)
		{
			return entry.bPhysicsComponent == physicsComponent;
		});

	if (iter != mPhysicsEntries.end())
	{
		mPhysicsEntries.erase(iter);
	}
}

void BPhysicsService::AddRigidBody(RigidBodyComponent* rigidBodyComponent)
{
	if (rigidBodyComponent->GetRigidBody() != nullptr)
	{
		mDynamicWorld->addRigidBody(rigidBodyComponent->GetRigidBody(), CollisionGroups::Default, CollisionGroups::Default);
	}
}

void BPhysicsService::RemoveRigidBody(RigidBodyComponent* rigidBodyComponent)
{
	if (rigidBodyComponent->GetRigidBody() != nullptr)
	{
		mDynamicWorld->removeRigidBody(rigidBodyComponent->GetRigidBody());
	}
}
