#include "Precompiled.h"
#include "App.h"

#include "AppState.h"

using namespace SAGE;
using namespace SAGE::Core;
using namespace SAGE::Graphics;
using namespace SAGE::Input;
using namespace SAGE::Coroutine;

void App::ChangeState(const std::string& stateName)
{
	auto iter = mAppStates.find(stateName);
	if (iter != mAppStates.end()) {
		LOG("App -- Next State: %s", stateName.c_str());
		mNextState = iter->second.get();
	}
}

void App::Run(AppConfig appConfig)
{
	LOG("App -- App Running");

	LOG("App -- Creating App Window...");
	Window window;
	window.Initialize(GetModuleHandle(nullptr), appConfig.appName, appConfig.winWidth, appConfig.winHeight);

	LOG("App -- Initializing engine systems...");
	auto handle = window.GetWindowHandle();
	InputSystem::StaticInitialize(handle);
	GraphicsSystem::StaticInitialize(handle, false);
	SimpleDraw::StaticInitialize(appConfig.debugDrawLimit);
	DebugUI::StaticInitialize(handle, false, true);
	TextureManager::StaticInitialize(appConfig.textureRoot);
	ModelManager::StaticInitialize();
	SpriteRenderer::StaticInitialize();
	AudioSystem::StaticInitialize();
	SoundEffectManager::StaticInitialize(appConfig.audioRoot.relative_path().string().c_str());
	Font::StaticInitialize();
	CoroutineSystem::StaticInitialize();

	LOG("App -- Initializing app state...");
	ASSERT(mCurrentState, "App -- No app state found");
	mCurrentState->Initialize();

	LOG("App -- Entering Game Loop...");
	mRunning = true;
	while (mRunning)
	{
		window.ProcessMessage();
		if (!window.IsActive()) {
			Quit();
			continue;
		}

		auto inputSystem = InputSystem::Get();
		inputSystem->Update();

		if (inputSystem->IsKeyPressed(KeyCode::ESCAPE)) {
			Quit();
			continue;
		}

		// Update audio
		AudioSystem::Get()->Update();

		if (mNextState) {
			mCurrentState->Terminate();
			mCurrentState = std::exchange(mNextState, nullptr);
			mCurrentState->Initialize();
		}

		// Update current state
		auto deltaTime = TimeUtil::GetDeltaTime();
		mCurrentState->Update(deltaTime);

		// Coroutine
		CoroutineSystem::Get()->Update();

		// Other
		auto graphicsSystem = GraphicsSystem::Get();
		auto spriteRenderer = SpriteRenderer::Get();
		graphicsSystem->BeginRender();
		spriteRenderer->BeginRender();
		mCurrentState->Render();
		spriteRenderer->EndRender();

		DebugUI::BeginRender();
		mCurrentState->DebugUI();
		DebugUI::EndRender();

		graphicsSystem->EndRender();
	}

	LOG("App -- Terminating App State...");
	mCurrentState->Terminate();

	LOG("App -- Shutting down engine systems...");
	CoroutineSystem::StaticTerminate();
	Font::StaticTerminate();
	SoundEffectManager::StaticTerminate();
	AudioSystem::StaticTerminate();
	SpriteRenderer::StaticTerminate();
	ModelManager::StaticTerminate();
	TextureManager::StaticTerminate();
	DebugUI::StaticTerminate();
	SimpleDraw::StaticTerminate();
	GraphicsSystem::StaticTerminate();
	InputSystem::StaticTerminate();
	
	LOG("App -- Terminating Window...");
	window.Terminate();
}

void App::Quit()
{
	mRunning = false;
}