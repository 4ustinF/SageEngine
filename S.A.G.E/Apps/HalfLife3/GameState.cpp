#include "GameState.h"

#include "DoorITVComponent.h"
#include "PlayerControllerComponent.h"
#include "TextureAnimatorComponent.h"
#include "ToggleTriggerVolumeComponent.h"

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
		if (strcmp(componentName, "Door Interact Toggle Volume Component") == 0)
		{
			DoorITVComponent* doorITVComponent = gameObject.AddComponent<DoorITVComponent>();
			doorITVComponent->LoadComponentFromTemplate(value);
			return true;
		}
		else if (strcmp(componentName, "Texture Animator Component") == 0)
		{
			TextureAnimatorComponent* textureAnimatorComponent = gameObject.AddComponent<TextureAnimatorComponent>();
			textureAnimatorComponent->LoadComponentFromTemplate(value);
			return true;
		}
		else if (strcmp(componentName, "Toggle Trigger Volume Component") == 0)
		{
			ToggleTriggerVolumeComponent* toggleTriggerVolumeComponent = gameObject.AddComponent<ToggleTriggerVolumeComponent>();
			toggleTriggerVolumeComponent->LoadComponentFromTemplate(value);
			return true;
		}
		else if (strcmp(componentName, "Player Controller Component") == 0)
		{
			PlayerControllerComponent* playerControllerComponent = gameObject.AddComponent<PlayerControllerComponent>();
			playerControllerComponent->LoadComponentFromTemplate(value);
			return true;
		}
		return false;
	}
}

void GameState::Initialize()
{
	mCameraService = mGameWorld.AddService<CameraService>();
	RenderService* renderService = mGameWorld.AddService<RenderService>();
	RBPhysicsService* physicsService = mGameWorld.AddService<RBPhysicsService>();
	mGameWorld.Initialize(1000);
	mGameWorld.SetEditMode(true);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/HalfLife/c1a0_level.json");

	GameObject* barneyGO = mGameWorld.CreateGameObject("../../Assets/Templates/HalfLifeTemplates/Characters/barney.json");
	barneyGO->SetName("Barney");
	AnimatorComponent* barneyGOAnimatorComp = barneyGO->GetComponent<AnimatorComponent>();
	ModelComponent* barneyGOModelComp = barneyGO->GetComponent<ModelComponent>();
	Animator& barneyGOAnimator = barneyGOAnimatorComp->GetAnimator();
	barneyGOAnimator.PlayAnimation(static_cast<int>(barneyGOModelComp->GetModel().animationSet.size()) -1, true);

	// TODO: Move over to level asset.
	renderService->SetSampleFilter(Sampler::Filter::Point);
	renderService->GetDirectionalLight().direction = Math::Normalize(Vector3(0.0f, -1.0f, 0.0f));
	renderService->GetDirectionalLight().ambient = Colors::White;
	renderService->GetDirectionalLight().diffuse = Colors::White;

	// TODO: Move over to level asset.
	physicsService->SetRenderDebugUI(true);

	//auto tm = TextureManager::Get();
	//mGlassEffect.SetBlendState(BlendState::Mode::AlphaBlend);
	//mGlassEffect.Initialize(Sampler::Filter::Linear);
	//mGlassEffect.SetLightCamera(renderService->GetShadowEffect().GetLightCamera());
	//mGlassEffect.SetDirectionalLight(renderService->GetDirectionalLight());
	//mGlassEffect.SetShadowMap(&renderService->GetShadowEffect().GetDepthMap());
	//mGlassEffect.SetDepthBias(0.000021f);
	//mGlassEffect.SetBumpWeight(0.25f);
	//mGlassEffect.SetSampleSize(0);
	//mGlassEffect.SetShatterNormalMapId(tm->LoadTexture("RadialCracks2.jpg"));
	//mGlassEffect.SetShatterIntensity(2.0f);
	////mGlassEffect.SetShatterNormalMapId();
	//mGlassEffect.SetCamera(mCameraService->GetCamera());

	//mGlassRenderObject.material.ambient = { 0.8f, 0.8f, 0.8f, 0.5f };
	//mGlassRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	//mGlassRenderObject.material.specular = { 0.8f, 0.8f, 0.8f, 1.0f };
	//mGlassRenderObject.material.emissive = { 1.0f, 1.0f, 1.0f, 0.0f };
	//mGlassRenderObject.material.power = 10.0f;
	//mGlassRenderObject.diffuseMapId = tm->LoadTexture("Glass.png");
	////mGlassRenderObject.meshBuffer.Initialize(MeshBuilder::CreateQuad(1.0f, 1.0f));
	//mGlassRenderObject.meshBuffer.Initialize(MeshBuilder::CreateGlass(1.0f, 1.0f));

	PhysicsWorld::Settings settings;
	settings.iterations = 10;
	settings.drag = 0.1f;
	mPhysicsWorld.Initialize(settings);
}

void GameState::Terminate()
{
	mGlassEffect.Terminate();
	mCameraService = nullptr;
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mPhysicsWorld.Update(deltaTime);
	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	mGameWorld.Render();

	//mGlassEffect.Begin();
	//mGlassEffect.Render(mGlassRenderObject);
	//mGlassEffect.End();
}

void GameState::DebugUI()
{
	mPhysicsWorld.DebugDraw();
	mGameWorld.DebugUI();

	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	//mGlassEffect.DebugUI();

	//ImGui::DragFloat3("Position##", &mGlassRenderObject.transform.position.x, 0.1f);
	//ImGui::DragFloat3("Scale##", &mGlassRenderObject.transform.scale.x, 0.1f);
	//ImGui::ColorEdit4("Ambient##Floor", &mGlassRenderObject.material.ambient.r);
	//ImGui::ColorEdit4("Diffuse##Floor", &mGlassRenderObject.material.diffuse.r);
	//ImGui::ColorEdit4("Specular##Floor", &mGlassRenderObject.material.specular.r);
	//ImGui::ColorEdit4("Emissive##Floor", &mGlassRenderObject.material.emissive.r);
	//ImGui::DragFloat("Power##Floor", &mGlassRenderObject.material.power, 1.0f, 1.0f, 100.0f);

	ImGui::End();
}
