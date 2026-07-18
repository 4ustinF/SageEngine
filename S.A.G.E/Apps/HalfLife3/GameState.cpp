#include "GameState.h"
#include "TextureAnimatorComponent.h"

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
		if (strcmp(componentName, "Texture Animator Component") == 0)
		{
			TextureAnimatorComponent* textureAnimatorComponent = gameObject.AddComponent<TextureAnimatorComponent>();
			textureAnimatorComponent->LoadComponentFromTemplate(value);
			return true;
		}
		return false;
	}
}

void GameState::Initialize()
{
	mCameraService = mGameWorld.AddService<CameraService>();
	RenderService* renderService = mGameWorld.AddService<RenderService>();
	mGameWorld.Initialize(1000);
	mGameWorld.SetEditMode(true);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/HalfLife/halflife_level.json");

	GameObject* barneyGO = mGameWorld.CreateGameObject("../../Assets/Templates/HalfLifeTemplates/Characters/barney.json");
	barneyGO->SetName("Barney");
	barneyGO->GetComponent<AnimatorComponent>()->GetAnimator().PlayAnimation(1, false);

	// TODO: Move over to level asset.
	renderService->SetSampleFilter(Sampler::Filter::Point);
	renderService->GetDirectionalLight().direction = Math::Normalize(Vector3(0.0f, -1.0f, 0.0f));
	renderService->GetDirectionalLight().ambient = Colors::White;
	renderService->GetDirectionalLight().diffuse = Colors::White;

	// TODO:
	// Render service:
	// Update FPS less frequently
	// Grid settings like size, on/off, and y value?
}

void GameState::Terminate()
{
	mCameraService = nullptr;
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	mGameWorld.Render();
	SimpleDraw::Render(mCameraService->GetCamera());
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();

	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::End();
}
