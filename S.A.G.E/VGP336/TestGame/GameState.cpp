#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	//GraphicsSystem::Get()->SetClearColor(Colors::Black);

	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>();
	mGameWorld.AddService<TerrainService>();
	mGameWorld.Initialize(1000);

	mGameWorld.CreateGameObject("../../Assets/Templates/fps_camera.json");
	mGameObjectHandle = mGameWorld.CreateGameObject("../../Assets/Templates/test.json")->GetHandle();
	mGameWorld.CreateGameObject("../../Assets/Templates/player.json");
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