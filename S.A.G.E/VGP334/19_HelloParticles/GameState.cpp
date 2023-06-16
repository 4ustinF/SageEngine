#include "GameState.h"

#include <chrono>

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 5.0f, -20.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mParticleEffect.SetBlendState(BlendState::Mode::AlphaPremultiplied);
	mParticleEffect.Initialize();
	mParticleEffect.SetCamera(mCamera);

	mParticleSystem.Initialize();
	mParticleSystem.SetTextureSheet("Explosion.png", true, 7, 10);
}

void GameState::Terminate()
{
	mParticleSystem.Terminate();
	mParticleEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	mParticleSystem.CameraPos = mCamera.GetPosition();

	auto inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(KeyCode::M))
	{
		useMouseDebugMode = !useMouseDebugMode;
		inputSystem->ShowSystemCursor(useMouseDebugMode);
	}

	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 2.0f, 10.0f, 0.1f, deltaTime, useMouseDebugMode);
	mFPS = 1.0f / deltaTime;

	//deltaTime *= 0.05f;
	mParticleSystem.Update(deltaTime);
}

void GameState::Render()
{
	SimpleDraw::AddPlane(10, Colors::White);
	SimpleDraw::AddTransform(Matrix4::Identity);
	SimpleDraw::Render(mCamera);

	//auto start = std::chrono::high_resolution_clock::now();

	//mParticleSystem.BubbleSort();
	mParticleSystem.QuickSort(mParticleSystem.mParticles, 0, mParticleSystem.mParticles.size() - 1, mCamera.GetPosition());
	
	//auto stop = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

	mParticleEffect.Begin();
	{
		for (const auto& p : mParticleSystem.mParticles)
		{
			auto& quad = mParticleSystem.mParticleQuad;
			quad.transform.position = p.position;
			quad.transform.rotation = Quaternion::RotationLook(-mCamera.GetPosition()) * Quaternion::RotationAxis(Vector3::ZAxis, Constants::DegToRad * p.currentRotation);
			quad.transform.scale = Vector3(p.currentScale);
			mParticleEffect.SetParticleColor(p.currentColor);

			if (mParticleSystem.UsingTextureSheet())
			{
				// Get particle index
				// set uvs accordingly
				mParticleEffect.SetUVOffSet(mParticleSystem.UpdateUVs(p.index));
			}

			mParticleEffect.Render(mParticleSystem.mParticleQuad);
		}
	}
	mParticleEffect.End();
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("FPS: %f", mFPS);
	ImGui::Separator();
	mParticleSystem.DebugUI();

	ImGui::End();
}