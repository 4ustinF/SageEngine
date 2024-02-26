#include "GameState.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <ImGui/Inc/ImPlot.h>
#include "ImGui/Src/imgui_internal.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::ML;

namespace
{
	// Check for custom components
	bool OnMake(const char* componentName, const rapidjson::Value& value, GameObject& gameObject)
	{
		return false;
	}
}

namespace 
{
	struct ClusterResult
	{
		std::vector<Vector2> centroids;
		std::vector<int> assignment;
	};

	ClusterResult KMeansCluster(const std::vector<Vector2>& data, int K)
	{
		std::vector<Vector2> centroids;
		centroids.resize(K);
		for (auto& centroid : centroids)
		{
			centroid.x = Random::UniformFloat(0.0f, 800.0f);
			centroid.y = Random::UniformFloat(0.0f, 600.0f);
		}

		return {};
	}
}

void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/bare.json");

	mUnitTexture.Initialize(L"../../Assets/Sprites/X/scv_01.png");

	for (size_t i = 0; i < 20; ++i)
	{
		const float pX = Random::UniformFloat(0.0f, 800.0f);
		const float pY = Random::UniformFloat(0.0f, 600.0f);
		const float vX = Random::UniformFloat(-100.0f, 100.0f);
		const float vY = Random::UniformFloat(-100.0f, 100.0f);

		mUnits.emplace_back(Unit{ {pX, pY}, {vX, vY} });
	}
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
	mUnitTexture.Terminate();
}

void GameState::Update(float deltaTime)
{
	//mGameWorld.Update(deltaTime);

	const auto& inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyDown(KeyCode::SPACE))
	{
		// Run kMeans
		// Show centroids/assignment
	}
	else
	{
		for (auto& unit : mUnits)
		{
			unit.position += unit.velocity * deltaTime;
			if (unit.position.x < 0.0f) {
				unit.position.x += 800.0f;
			}
			if (unit.position.x > 800.0f) {
				unit.position.x -= 800.0f;
			}
			if (unit.position.y < 0.0f) {
				unit.position.y += 600.0f;
			}
			if (unit.position.y > 600.0f) {
				unit.position.y -= 600.0f;
			}
		}
	}

}

void GameState::Render()
{
	//mGameWorld.Render();
}

void GameState::DebugUI()
{
	//mGameWorld.DebugUI();

	ImGui::SetNextWindowSize({ 800.0f, 600.0f });
	ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoResize);

	const ImVec2 unitSize = ImVec2(32.0f, 32.0f);
	for (const auto& unit : mUnits)
	{
		ImGui::SetCursorPos(unit.position - (unitSize * 0.5f));
		ImGui::Image(mUnitTexture.GetRawData(), unitSize);
	}

	ImVec2 winPos = ImGui::GetWindowPos();
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	drawList->AddCircle(winPos + mUnits[0].position, 50.0f, 0xff0000ff);

	ImGui::End();
}
