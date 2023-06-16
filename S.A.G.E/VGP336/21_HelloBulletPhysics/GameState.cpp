#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

namespace
{
	// Check for custom components
	bool OnMake(const char* componentName, const rapidjson::Value& value, GameObject& gameObject)
	{
		return false;
	}
}

void GameState::Initialize()
{
	//GraphicsSystem::Get()->SetClearColor(Colors::Black);

	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<BPhysicsService>();
	mGameWorld.AddService<RenderService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);

	mGameWorld.CreateGameObject("../../Assets/Templates/fps_camera.json")->SetName("Camera");
	mGameWorld.CreateGameObject("../../Assets/Templates/test_player.json")->SetName("Player");
	mGameWorld.CreateGameObject("../../Assets/Templates/cube.json")->SetName("Ground");
	mGameWorld.CreateGameObject("../../Assets/Templates/cube2.json")->SetName("Cube");
	
	// Player
	auto playerRBC = mGameWorld.FindGameObject("Player")->GetComponent<RigidBodyComponent>();
	playerRBC->AddCollisionMask(CollisionGroups::Group1);
	//playerRBC->RemoveCollisionMask(CollisionGroups::Default);
	mPlayerRB = playerRBC->GetRigidBody();
	mPlayerRB->setActivationState(DISABLE_DEACTIVATION);

	// Ground
	auto groundRBC = mGameWorld.FindGameObject("Ground")->GetComponent<RigidBodyComponent>();
	groundRBC->SetCollisionGroup(CollisionGroups::Group1);
	mGroundRB = groundRBC->GetRigidBody();

	// Custom Mesh
	// Create a static mesh shape for a static object
	btTriangleMesh* triangleMesh = new btTriangleMesh();

	// Add triangles to the triangle mesh that represent the static object's shape
	// Define the vertices of the box
	const float halfExtent = 0.5f;
	btVector3 vertices[8] = {
		btVector3(-halfExtent, -halfExtent, -halfExtent),
		btVector3(halfExtent, -halfExtent, -halfExtent),
		btVector3(-halfExtent, halfExtent, -halfExtent),
		btVector3(halfExtent, halfExtent, -halfExtent),
		btVector3(-halfExtent, -halfExtent, halfExtent),
		btVector3(halfExtent, -halfExtent, halfExtent),
		btVector3(-halfExtent, halfExtent, halfExtent),
		btVector3(halfExtent, halfExtent, halfExtent)
	};

	// Define the indices of the triangles that make up the box faces
	int indices[36] = {
		0, 1, 3,
		3, 2, 0,
		1, 5, 7,
		7, 3, 1,
		5, 4, 6,
		6, 7, 5,
		4, 0, 2,
		2, 6, 4,
		2, 3, 7,
		7, 6, 2,
		4, 5, 1,
		1, 0, 4
	};

	// Add the triangles to the triangle mesh
	//for (int i = 0; i < 36; i += 3) {
	for (int i = 0; i < 12; i += 3) {
		btVector3 vertex1 = vertices[indices[i]];
		btVector3 vertex2 = vertices[indices[i + 1]];
		btVector3 vertex3 = vertices[indices[i + 2]];
		triangleMesh->addTriangle(vertex1, vertex2, vertex3);
	}

	// Create a btBvhTriangleMeshShape object from the triangle mesh
	btBvhTriangleMeshShape* staticShape = new btBvhTriangleMeshShape(triangleMesh, true);
	btVector3 localIntertia(0.0f, 0.0f, 0.0f);

	Transform Transform;
	btDefaultMotionState* motionState = new btDefaultMotionState(ConvertToBtTransform(Transform));
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, staticShape, localIntertia);
	mCustomRB = new btRigidBody(rbInfo);

	mCustomRB->setUserIndex(CollisionGroups::Default);

	auto physicsService = mGameWorld.GetService<BPhysicsService>();
	physicsService->GetDynamicsWorld()->addRigidBody(mCustomRB, CollisionGroups::Default, CollisionGroups::Default);
}

void GameState::Terminate()
{
	auto physicsService = mGameWorld.GetService<BPhysicsService>();
	physicsService->GetDynamicsWorld()->removeRigidBody(mCustomRB);
	if (mCustomRB->getMotionState())
	{
		delete mCustomRB->getMotionState();
	}
	SafeDelete(mCustomRB);

	mPlayerRB = nullptr;
	mGroundRB = nullptr;
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	auto inputSystem = InputSystem::Get();
	const float distance = 50.0f * deltaTime;
	const float maxSpeed = 3.5f;

	if (inputSystem->IsKeyDown(KeyCode::UP))
	{
		if (mPlayerRB->getLinearVelocity().z() < maxSpeed)
		{
			mPlayerRB->applyCentralImpulse(btVector3(0.0f, 0.0f, distance));
		}
	}

	if (inputSystem->IsKeyDown(KeyCode::DOWN))
	{
		if (mPlayerRB->getLinearVelocity().z() > -maxSpeed)
		{
			mPlayerRB->applyCentralImpulse(btVector3(0.0f, 0.0f, -distance));
		}
	}

	if (inputSystem->IsKeyDown(KeyCode::RIGHT))
	{
		if (mPlayerRB->getLinearVelocity().x() < maxSpeed)
		{
			mPlayerRB->applyCentralImpulse(btVector3(distance, 0.0f, 0.0f));
		}
	}

	if (inputSystem->IsKeyDown(KeyCode::LEFT))
	{
		if (mPlayerRB->getLinearVelocity().x() > -maxSpeed)
		{
			mPlayerRB->applyCentralImpulse(btVector3(-distance, 0.0f, 0.0f));
		}
	}

	if (inputSystem->IsKeyPressed(KeyCode::SPACE))
	{
		mPlayerRB->applyCentralImpulse(btVector3(0.0f, 10.0f, 0.0f));
	}

	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	mGameWorld.Render();
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();
}