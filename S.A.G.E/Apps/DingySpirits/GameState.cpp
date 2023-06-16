#include "GameState.h"

#include "MutantControllerComponent.h"
#include "PlayerControllerComponent.h"

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
		if (strcmp(componentName, "MutantControllerComponent") == 0)
		{
			auto mutantControllerComponent = gameObject.AddComponent<MutantControllerComponent>();
			if (value.HasMember("WalkSpeed"))
			{
				const float moveSpeed = value["WalkSpeed"].GetFloat();
				mutantControllerComponent->SetWalkSpeed(moveSpeed);
			}
			if (value.HasMember("RunSpeed"))
			{
				const float moveSpeed = value["RunSpeed"].GetFloat();
				mutantControllerComponent->SetRunSpeed(moveSpeed);
			}
			return true;
		}
		if (strcmp(componentName, "PlayerControllerComponent") == 0)
		{
			auto playerControllerComponent = gameObject.AddComponent<PlayerControllerComponent>();
			if (value.HasMember("WalkSpeed"))
			{
				const float moveSpeed = value["WalkSpeed"].GetFloat();
				playerControllerComponent->SetWalkSpeed(moveSpeed);
			}
			if (value.HasMember("RunSpeed"))
			{
				const float moveSpeed = value["RunSpeed"].GetFloat();
				playerControllerComponent->SetRunSpeed(moveSpeed);
			}
			return true;
		}

		return false;
	}
}

void GameState::Initialize()
{
	//GraphicsSystem::Get()->SetClearColor(Colors::Black);

	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>();
	mGameWorld.AddService<TerrainService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/test_level.json");
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	auto gameObject = mGameWorld.GetGameObject(mGameObjectHandle);
	if (gameObject != nullptr) {
		gameObject->DebugUI();
	}

	mGameWorld.Render();
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();
}