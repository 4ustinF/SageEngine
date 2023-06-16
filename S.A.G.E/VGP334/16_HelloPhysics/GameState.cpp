#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	//GraphicsSystem::Get()->SetClearColor(Colors::SteelBlue);

	mCamera.SetPosition({0.0f, 3.0f, -5.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});

	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
	mDirectionalLight.ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	mStandardEffect.Initialize();
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mStandardEffect.SetDepthBias(0.000023f);
	mStandardEffect.SetBumpWeight(0.25f);
	mStandardEffect.SetSampleSize(0);

	mShadowEffect.Initialize();
	mShadowEffect.SetDirectionalLight(mDirectionalLight);
	mShadowEffect.SetSize(50.0f);

	// Duck
	const char* modelFileName = "../../Assets/Models/Duck/duck.model";
	ModelIO::LoadModel(modelFileName, mDuckModel);
	ModelIO::LoadMaterial(modelFileName, mDuckModel);

	mDuck = CreateRenderGroup(mDuckModel);

	// Ground
	mGroundRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mGroundRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mGroundRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mGroundRenderObject.material.emissive = { 0.1f, 0.1f, 0.1f, 1.0f };
	mGroundRenderObject.material.power = 10.0f;
	mGroundRenderObject.diffuseMapId = TextureManager::Get()->LoadTexture("concrete.jpg");
	mGroundRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(25, 25, 1.0f));

	PhysicsWorld::Settings settings;
	settings.iterations = 10;
	settings.drag = 0.1f;
	mPhysicsWorld.Initialize(settings);

	mPhysicsWorld.AddPlane({Vector3::YAxis, 0.0f});
	////mPhysicsWorld.AddOBB({ {3.0f, 3.0f, 0.0f}, {3.0f, 0.2f, 2.0f}, Quaternion::RotationEuler({0.0f, 0.0f, -0.5f}) });
	//////mPhysicsWorld.AddOBB({ {5.5f, 1.5f, 0.0f}, {1.5f, 1.5f, 6.0f}, Quaternion::Identity });
	////mPhysicsWorld.AddAABB({ {5.5f, 1.5f, 0.0f}, {1.5f, 1.5f, 6.0f} });

	// Cloth
	mClothRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mClothRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mClothRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mClothRenderObject.material.emissive = { 0.7f, 0.7f, 0.7f, 1.0f };
	mClothRenderObject.material.power = 10.0f;
	mClothRenderObject.diffuseMapId = TextureManager::Get()->LoadTexture("Fabric.jpg");
	mClothRenderObject.normalMapId = TextureManager::Get()->LoadTexture("FabricNormal.jpg");
}

void GameState::Terminate()
{
	mGroundRenderObject.Terminate();
	CleanUpRenderGroup(mDuck);
	mShadowEffect.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 2.0f, 10.0f, 0.1f, deltaTime);
	mFPS = 1.0f / deltaTime;
	
	if (InputSystem::Get()->IsMousePressed(MouseButton::LBUTTON))
	{
		auto p = mPhysicsWorld.AddParticle(0.25, 1.0f);
		p->SetPosition(mCamera.GetPosition());
		p->SetVelocity(mCamera.GetDirection() * 0.5f);
		p->bounce = 0.5f;
		p->radius = 0.5f;
	}

	mPhysicsWorld.Update(deltaTime);

	if (!mClothParticles.empty())
	{
		for (int i = 0; i < mClothParticles.size(); ++i)
		{
			mMesh.vertices[i].position = mClothParticles[i]->position;
		}

		mClothRenderObject.meshBuffer.Terminate();
		mClothRenderObject.meshBuffer.Initialize(mMesh);
	}
}

void GameState::Render()
{
	mPhysicsWorld.DebugDraw();

	if (!mTetrahedrons.empty())
	{
		mShadowEffect.Begin();
		for (auto& tetrahedrons : mTetrahedrons)
		{
			auto worldMatrix = tetrahedrons.GetWorldMatrix();
			auto translation = GetTranslation(worldMatrix);
			auto rotation = Quaternion::RotationMatrix(worldMatrix);
			for (auto& robj : mDuck)
			{
				robj.transform.rotation = rotation;
				robj.transform.position = GetTranslation(worldMatrix);
			}
			mShadowEffect.Render(mDuck);
		}
		mShadowEffect.End();

		mStandardEffect.Begin();
		for (auto& tetrahedrons : mTetrahedrons)
		{
			auto worldMatrix = tetrahedrons.GetWorldMatrix();
			auto translation = GetTranslation(worldMatrix);
			auto rotation = Quaternion::RotationMatrix(worldMatrix);
			for (auto& robj : mDuck)
			{
				robj.transform.rotation = rotation;
				robj.transform.position = GetTranslation(worldMatrix);
			}
			mStandardEffect.Render(mDuck);
		}
		mStandardEffect.End();
	}

	mStandardEffect.Begin();
	//mStandardEffect.Render(mGroundRenderObject);
	mStandardEffect.Render(mClothRenderObject);
	mStandardEffect.End();

	SimpleDraw::AddTransform(Matrix4::Identity);
	SimpleDraw::AddPlane(20, Colors::White);
	SimpleDraw::Render(mCamera);
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("FPS: %f", mFPS);
	ImGui::Separator();

	if(ImGui::Button("Boom!"))
	{
		mPhysicsWorld.Clear(true);
		mTetrahedrons.clear();
		for (int i = 0; i < 100; ++i)
		{
			Vector3 vel = Random::OnUnitSphere() * 0.1f;
			vel.y = Abs(vel.y);
			//vel *= 1 / 60.0f;

			auto particle = mPhysicsWorld.AddParticle();
			particle->SetPosition({0.0f, 2.0f, 0.0f});
			particle->SetVelocity(vel);
			particle->radius = 0.02f;
			particle->invMass = 1.0f;
			particle->bounce = Random::UniformFloat(0.5f, 0.95f);
		}
	}

	if (ImGui::Button("Sticks!"))
	{
		mPhysicsWorld.Clear(true);
		mTetrahedrons.clear();
		for (int i = 0; i < 100; ++i)
		{
			Vector3 vel1 = Random::OnUnitSphere() * 0.1f;
			vel1.y = Abs(vel1.y);

			Vector3 vel2 = Random::OnUnitSphere() * 0.1f;
			vel2.y = Abs(vel2.y);

			const float bounce = Random::UniformFloat(0.5f, 0.95f);

			auto particle1 = mPhysicsWorld.AddParticle();
			particle1->SetPosition({ -0.1f, 1.0f, 0.0f });
			particle1->SetVelocity(vel1);
			particle1->radius = 0.02f;
			particle1->invMass = 1.0f;
			particle1->bounce = bounce;

			auto particle2 = mPhysicsWorld.AddParticle();
			particle2->SetPosition({ 0.1f, 1.0f, 0.0f });
			particle2->SetVelocity(vel2);
			particle2->radius = 0.02f;
			particle2->invMass = 1.0f;
			particle2->bounce = bounce;

			mPhysicsWorld.AddConstraint<Physics::Spring>(particle1, particle2);
		}
	}

	if (ImGui::Button("Tetrahedron!"))
	{
		mPhysicsWorld.Clear(true);
		mTetrahedrons.clear();
		for (int i = 0; i < 50; ++i)
		{
			const float bounce = Random::UniformFloat(0.5f, 0.95f);

			auto p1 = mPhysicsWorld.AddParticle();
			p1->SetPosition({ 0.0f, 1.0f, 0.0f });
			p1->radius = 0.02f;
			p1->invMass = 1.0f;
			p1->bounce = bounce;

			auto p2 = mPhysicsWorld.AddParticle();
			p2->SetPosition({ 0.0f, 0.8f, 0.0f });
			p2->radius = 0.02f;
			p2->invMass = 1.0f;
			p2->bounce = bounce;

			auto p3 = mPhysicsWorld.AddParticle();
			p3->SetPosition({ 0.1f, 0.8f, -0.1f });
			p3->radius = 0.02f;
			p3->invMass = 1.0f; 
			p3->bounce = bounce;

			auto p4 = mPhysicsWorld.AddParticle();
			p4->SetPosition({ -0.1f, 0.8f, 0.1f });
			p4->radius = 0.02f;
			p4->invMass = 1.0f;
			p4->bounce = bounce;

			Vector3 vel = Random::OnUnitSphere() * 0.25f;
			vel.y = Abs(vel.y) * 2.0f;
			p1->SetVelocity(vel);

			mPhysicsWorld.AddConstraint<Physics::Spring>(p1, p2, 0.2f);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p1, p3, 0.2f);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p1, p4, 0.2f);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p2, p3, 0.2f);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p3, p4, 0.2f);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p4, p2, 0.2f);
		}
	}

	if (ImGui::Button("Ball and chain!"))
	{
		mPhysicsWorld.Clear(true);
		mTetrahedrons.clear();

		Math::Vector3 position{0.0f, 6.0f , 0.0f};
		Physics::Particle* last = nullptr;
		for (int i = 0; i < 10; ++i)
		{
			auto p = mPhysicsWorld.AddParticle();
			p->SetPosition(position);
			p->SetVelocity(Vector3::Zero);
			p->radius = (i == 9) ? 0.5f : 0.02f;
			p->invMass = (i == 9) ? 0.1f : 1.0f;

			if (i == 0) {
				mPhysicsWorld.AddConstraint<Physics::Fixed>(p);
			}
			else {
				mPhysicsWorld.AddConstraint<Physics::Spring>(p, last);
			}

			position.x += 0.5f;
			last = p;
		}
	}

	if (ImGui::Button("Ducks!"))
	{
		mPhysicsWorld.Clear(true);
		mTetrahedrons.clear();

		for (int i = 0; i < 50; ++i)
		{
			auto& t = mTetrahedrons.emplace_back(mPhysicsWorld.AddTetrahedron());
			t.SetPosition({0.0f, 1.0f, 0.0f});
			Vector3 vel = Random::OnUnitSphere() * 0.1f;
			vel.y = Abs(vel.y) * 1.25f;
			t.SetVelocity(vel);
			t.a->SetVelocity(Random::OnUnitSphere() * 0.25f);

			const float bounce = Random::UniformFloat(0.5f, 0.95f);
			t.a->bounce = bounce;
			t.b->bounce = bounce;
			t.c->bounce = bounce;
			t.d->bounce = bounce;
		}
	}

	if (ImGui::Button("Cloth!"))
	{
		mPhysicsWorld.Clear(true);
		mTetrahedrons.clear();
		mClothParticles.clear();

		const int column = 25;
		const int rows = 20;

		const float spacing = 0.1f;
		const float xOffSet = column * spacing * 0.5f;
		const float yOffSet = 1.0f;

		for (int i = 0; i <= rows; ++i)
		{
			for (int j = 0; j <= column; ++j)
			{
				auto p = mPhysicsWorld.AddParticle();

				p->SetPosition({ j * spacing - xOffSet, i * spacing + yOffSet, 0.0f });
				p->SetVelocity(Vector3::Zero);
				p->radius = 0.02f;
				p->invMass = 0.5f;

				const float u = 1.0f - (1.0f / column) * j;
				const float v = (1.0f / rows) * i;
				mMesh.vertices.push_back({ p->position, Vector3::ZAxis, Vector3::XAxis, {u, v}});

				if (i != rows && j != column) {
					mMesh.indices.push_back(j + i + (i * column) + 2 + column);		//TopRight
					mMesh.indices.push_back(j + i + (i * column) + 1);				//Bottom Right
					mMesh.indices.push_back(j + i + (i * column));					//Bottom Left

					mMesh.indices.push_back(j + i + (i * column) + 2 + column);		//TopRight
					mMesh.indices.push_back(j + i + (i * column));					//Bottom Left
					mMesh.indices.push_back(j + i + (i * column) + 1 + column);		//TopLeft

					mMesh.indices.push_back(j + i + (i * column));					//Bottom Left
					mMesh.indices.push_back(j + i + (i * column) + 1);				//Bottom Right
					mMesh.indices.push_back(j + i + (i * column) + 2 + column);		//TopRight

					mMesh.indices.push_back(j + i + (i * column) + 1 + column);		//TopLeft
					mMesh.indices.push_back(j + i + (i * column));					//Bottom Left
					mMesh.indices.push_back(j + i + (i * column) + 2 + column);		//TopRight
				}

				mClothParticles.emplace_back(p);
			}
		}
		mClothRenderObject.meshBuffer.Initialize(mMesh);

		mPhysicsWorld.AddConstraint<Physics::Fixed>(mClothParticles[mClothParticles.size() - 1]);
		mPhysicsWorld.AddConstraint<Physics::Fixed>(mClothParticles[mClothParticles.size() - column - 1]);

		for (int i = 0; i < mClothParticles.size() - 1; ++i)
		{
			// Right
			if ((i + 1) % (column + 1) != 0) {
				mPhysicsWorld.AddConstraint<Physics::Spring>(mClothParticles[i], mClothParticles[i + 1]);
			}

			// Top
			if (i + column + 1 < mClothParticles.size()) {
				mPhysicsWorld.AddConstraint<Physics::Spring>(mClothParticles[i], mClothParticles[i + column + 1]);
			}

			// Top Right
			if (i + column + 2 < mClothParticles.size() && (i + 1) % (column + 1) != 0) {
				mPhysicsWorld.AddConstraint<Physics::Spring>(mClothParticles[i], mClothParticles[i + column + 2]);
			}

			// Top left
			if (i + column < mClothParticles.size() && (i + column + 1) % (column + 1) != 0) {
				mPhysicsWorld.AddConstraint<Physics::Spring>(mClothParticles[i], mClothParticles[i + column]);
			}
		}
	}

	if (ImGui::Button("Cube!"))
	{
		mPhysicsWorld.Clear(true);
		mTetrahedrons.clear();
		for (int i = 0; i < 50; ++i)
		{
			auto p1 = mPhysicsWorld.AddParticle(0.02f, 1.0f);
			auto p2 = mPhysicsWorld.AddParticle(0.02f, 1.0f);
			auto p3 = mPhysicsWorld.AddParticle(0.02f, 1.0f);
			auto p4 = mPhysicsWorld.AddParticle(0.02f, 1.0f);
			auto p5 = mPhysicsWorld.AddParticle(0.02f, 1.0f);
			auto p6 = mPhysicsWorld.AddParticle(0.02f, 1.0f);
			auto p7 = mPhysicsWorld.AddParticle(0.02f, 1.0f);
			auto p8 = mPhysicsWorld.AddParticle(0.02f, 1.0f);

			p1->SetPosition({ -0.5f, -0.5f, -0.5f });
			p2->SetPosition({ -0.5f, +0.5f, -0.5f });
			p3->SetPosition({ +0.5f, +0.5f, -0.5f });
			p4->SetPosition({ +0.5f, -0.5f, -0.5f });
			p5->SetPosition({ -0.5f, -0.5f, +0.5f });
			p6->SetPosition({ -0.5f, +0.5f, +0.5f });
			p7->SetPosition({ +0.5f, +0.5f, +0.5f });
			p8->SetPosition({ +0.5f, -0.5f, +0.5f });

			const float bounce = Random::UniformFloat(0.5f, 0.95f);
			p1->bounce = bounce;
			p2->bounce = bounce;
			p3->bounce = bounce;
			p4->bounce = bounce;
			p5->bounce = bounce;
			p6->bounce = bounce;
			p7->bounce = bounce;
			p8->bounce = bounce;

			Vector3 vel = Random::OnUnitSphere() * 0.5f;
			vel.y = Abs(vel.y) * 3.0f;
			p1->SetVelocity(vel);

			mPhysicsWorld.AddConstraint<Physics::Spring>(p1, p2);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p2, p3);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p3, p4);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p4, p1);

			mPhysicsWorld.AddConstraint<Physics::Spring>(p5, p6);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p6, p7);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p7, p8);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p8, p5);

			mPhysicsWorld.AddConstraint<Physics::Spring>(p1, p5);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p2, p6);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p3, p7);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p4, p8);

			mPhysicsWorld.AddConstraint<Physics::Spring>(p1, p7);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p2, p8);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p3, p5);
			mPhysicsWorld.AddConstraint<Physics::Spring>(p4, p6);
		}
	}

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (ImGui::DragFloat3("Direction##Light", &mDirectionalLight.direction.x, 0.01f, -1.0f, 1.0f)) {
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}

		ImGui::ColorEdit4("Ambient##Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##Light", &mDirectionalLight.specular.r);
	}
	
	if (ImGui::CollapsingHeader("Ground", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::ColorEdit4("Ambient##Ground", &mGroundRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Ground", &mGroundRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Ground", &mGroundRenderObject.material.specular.r);
		ImGui::ColorEdit4("Emissive##Ground", &mGroundRenderObject.material.emissive.r);
		ImGui::DragFloat("Power##Ground", &mGroundRenderObject.material.power, 1.0f, 1.0f, 100.0f);
	}

	if (ImGui::CollapsingHeader("ClothRenderObject", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::ColorEdit4("Ambient##Cloth", &mClothRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Cloth", &mClothRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Cloth", &mClothRenderObject.material.specular.r);
		ImGui::ColorEdit4("Emissive##Cloth", &mClothRenderObject.material.emissive.r);
		ImGui::DragFloat("Power##Cloth", &mClothRenderObject.material.power, 1.0f, 1.0f, 100.0f);
	}

	if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::ColorEdit4("Ambient##Model", &mDuck[0].material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Model", &mDuck[0].material.diffuse.r);
		ImGui::ColorEdit4("Specular##Model", &mDuck[0].material.specular.r);
		ImGui::ColorEdit4("Emissive##Model", &mDuck[0].material.emissive.r);
		ImGui::DragFloat("Power##Model", &mDuck[0].material.power, 1.0f, 1.0f, 100.0f);
	}

	// Includes
	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mShadowEffect.DebugUI();
	ImGui::Separator();

	// Homework
	// Add button to spawn cloth
	// use tetrahedron to get transforms to draw a 3d model 
	// Use meshbufer update so you can dynamically update the vertex position/normals using the physics particles

	//Mesh clothMesh;
	//clothMesh.vertices;
	//clothMesh.indices;

	//RenderObject cloth;
	//cloth.meshBuffer.Update();

	// Includes
	ImGui::End();
	mPhysicsWorld.DebugUI();
}