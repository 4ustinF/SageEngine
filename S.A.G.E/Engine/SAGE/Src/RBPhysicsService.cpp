#include "Precompiled.h"
#include "RBPhysicsService.h"

#include "CameraService.h"
#include "GameObject.h"
#include "GameWorld.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;

void RBPhysicsService::Initialize()
{
	SetServiceName("RBPhysics Service");
	
	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
	mDirectionalLight.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	mStandardEffect.Initialize(Sampler::Filter::Linear);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetDepthBias(0.000021f);
	mStandardEffect.SetBumpWeight(0.25f);
	mStandardEffect.SetSampleSize(0);

	mPhysicsWorld.Initialize();

	const Vector3 ballPos = Vector3(0.0f, 10.0f, 0.0f);
	const float ballRadius = 1.0f;
	mPhysicsObject1 = new RBPhysicsObject(new BoundingSphere(ballPos, ballRadius), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -9.81, 0.0f));
	//mPhysicsObject1 = new RBPhysicsObject(new BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 1.0f), Vector3(0.0f, 0.0f, 1.141f / 2.0f));
	//mPhysicsObject2 = new RBPhysicsObject(new BoundingSphere(Vector3(1.414f / 2.0f * 7.0f, 0.0f, 1.414f / 2.0f * 7.0f), 1.0f), Vector3(-1.414f / 2.0f, 0.0f, -1.414f / 2.0f));
	mPhysicsWorld.AddObject(*mPhysicsObject1);
	//mPhysicsWorld.AddObject(*mPhysicsObject2);

	// Ball
	// TODO: We need a mesh renderer comp.
	auto tm = TextureManager::Get();
	mBallRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mBallRenderObject.material.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	mBallRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mBallRenderObject.material.power = 10.0f;
	mBallRenderObject.diffuseMapId = tm->LoadTexture("basketball.jpg");
	mBallRenderObject.meshBuffer.Initialize(MeshBuilder::CreateSphere(512, 1024, ballRadius));
	mBallRenderObject.transform.position = ballPos;
	mBallRenderObject.transform.rotation = Quaternion::RotationLook(Vector3(1.0f, 0.0f, 1.0f));
}

void RBPhysicsService::Terminate()
{
	mStandardEffect.Terminate();
	mBallRenderObject.Terminate();
	mPhysicsWorld.Clear();
}

void RBPhysicsService::Update(float deltaTime)
{
	mPhysicsWorld.Update(deltaTime);

	auto physicsObject = mPhysicsWorld.GetPhysicsObject(0);
	mBallRenderObject.transform.position = physicsObject.GetPosition();
	mBallRenderObject.transform.rotation = physicsObject.GetOrientation();
}

void RBPhysicsService::Render()
{
	//mPhysicsWorld.DebugDraw(); // ?
	auto& camera = GetWorld().GetService<CameraService>()->GetCamera();
	mStandardEffect.SetCamera(camera);

	mStandardEffect.Begin();
	mStandardEffect.Render(mBallRenderObject);
	mStandardEffect.End();
}

void RBPhysicsService::DebugUI()
{
	ImGui::Checkbox("Render Physics##RBPhysics", &mRenderDebugUI);
	//mPhysicsWorld.DebugDraw();
	mPhysicsWorld.DebugUI();
}
