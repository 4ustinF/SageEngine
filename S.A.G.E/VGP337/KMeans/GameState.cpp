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
		std::vector<Vector3> centroids;
		std::vector<int> assignments;

		std::vector<std::vector<Vector3>> centroidHistory;
	};

	ClusterResult KMeansCluster(const std::vector<Vector3>& data, int K, size_t maxIteration)
	{
		ASSERT(K <= data.size(), "Not Enough data for %zu clusters", K);

		ClusterResult result;

		// Use existing data points as starting cluster centroids
		result.centroids.resize(K);
		result.centroidHistory.resize(K);
		for (size_t i = 0; i < K; ++i)
		{
			result.centroids[i] = data[i];
		}

		for (size_t iteration = 0; iteration < maxIteration; ++iteration)
		{
			std::vector<Vector3> newCentroids;
			std::vector<size_t> clusterEntryCount;
			newCentroids.resize(K);
			clusterEntryCount.resize(K);

			// Assign cluster membership
			result.assignments.clear();
			result.assignments.reserve(data.size());
			for (const auto& sample : data)
			{
				size_t closestClusterIndex = 0;
				float closestDistanceSqr = DistanceSqr(sample, result.centroids[0]);
				for (size_t i = 1; i < result.centroids.size(); ++i)
				{
					const float distanceSqr = DistanceSqr(sample, result.centroids[i]);
					if (distanceSqr < closestDistanceSqr)
					{
						closestClusterIndex = i;
						closestDistanceSqr = distanceSqr;
					}
				}
				result.assignments.push_back(closestClusterIndex);
				newCentroids[closestClusterIndex] += sample;
				clusterEntryCount[closestClusterIndex]++;
			}

			// Compute new centroid
			for (size_t i = 0; i < newCentroids.size(); ++i) {
				newCentroids[i] /= static_cast<float>(clusterEntryCount[i]);
			}

			// Get history
			for (size_t i = 0; i < result.centroids.size(); ++i)
			{
				result.centroidHistory[i].push_back(result.centroids[i]);
			}

			// Check if we are done
			if (newCentroids == result.centroids) {
				break;
			}

			// Update centroid positions and repeat
			result.centroids = std::move(newCentroids);
		}

		return result;
	}

	ClusterResult kMeansResult;
}

void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/bare.json");

	mUnitTexture.Initialize(L"../../Assets/Sprites/X/scv_01.png");
	for (size_t i = 0; i < 30; ++i)
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

	if (!mShowKMeans)
	{
		if (inputSystem->IsKeyDown(KeyCode::SPACE))
		{
			mShowKMeans = true;

			std::vector<Vector3> data;
			data.reserve(mUnits.size());
			for (auto& unit : mUnits)
			{
				data.push_back({ unit.position.x, unit.position.y, 0.0f });
			}
			kMeansResult = KMeansCluster(data, 3, 10);
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
	else if (!inputSystem->IsKeyDown(KeyCode::SPACE))
	{
		mShowKMeans = false;
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

	const ImVec2 winPos = ImGui::GetWindowPos();
	ImDrawList* drawList = ImGui::GetForegroundDrawList();

	if (mShowKMeans)
	{
		const ImU32 clusterColors[] = { 0xffff8800, 0xff66ff44, 0xffff22ff };

		// Circles
		for (size_t i = 0; i < kMeansResult.centroids.size(); ++i)
		{
			const ImVec2 center{ kMeansResult.centroids[i].x, kMeansResult.centroids[i].y };
			drawList->AddCircle(winPos + center, 10.0f, clusterColors[i]);
		}

		// Lines
		for (size_t i = 0; i < kMeansResult.assignments.size(); ++i)
		{
			const size_t clusterIndex = kMeansResult.assignments[i];
			const Vector3 centroid = kMeansResult.centroids[clusterIndex];
			const ImVec2 center{ centroid.x, centroid.y };
			drawList->AddLine(winPos + center, winPos + mUnits[i].position, clusterColors[clusterIndex]);
		}

		for (size_t i = 0; i < kMeansResult.centroidHistory.size(); ++i)
		{
			for (size_t j = 0; j < kMeansResult.centroidHistory[i].size(); ++j)
			{
				const float radius = Lerp(3.0f, 7.5f, static_cast<float>(j) / static_cast<float>(kMeansResult.centroidHistory[i].size()));
				const auto& history = kMeansResult.centroidHistory[i][j];
				drawList->AddCircleFilled(winPos + ImVec2{ history.x, history.y }, radius, clusterColors[i]);

				if (j > 0)
				{
					const auto& lastHistory = kMeansResult.centroidHistory[i][j - 1];
					drawList->AddLine(winPos + ImVec2{ history.x, history.y }, winPos + ImVec2{ lastHistory.x, lastHistory.y }, 0xffffffff);
				}
			}
		}
	}

	ImGui::End();
}
