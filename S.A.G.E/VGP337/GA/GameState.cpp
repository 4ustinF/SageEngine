#include "GameState.h"

#include "ImGui/Inc/ImPlot.h"
#include "ImGui/Src/imgui_internal.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::ML;

namespace
{
	const std::string kTarget = "Genetic Algorithm is coooooool!";
	const int kValidGeneValue = 94; // 126 - 32; // From character '~' to ' ' in ASCII table
}

namespace
{
	// Check for custom components
	bool OnMake(const char* componentName, const rapidjson::Value& value, GameObject& gameObject)
	{
		return false;
	}
}

void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/bare.json");


}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	//mGameWorld.Update(deltaTime);

	const auto& inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(KeyCode::SPACE))
	{
		mAppLog.clear();
	}

	if (mInitialized)
	{
		const auto& best = mGeneticAlgorithm.GetBestGenome();
		if (best.fitness < static_cast<int>(kTarget.size()))
		{
			mGeneticAlgorithm.Advance();

			// Print the next best genome.
			std::string bestStr;
			for (const auto& gene : mGeneticAlgorithm.GetBestGenome().chromosome) {
				bestStr += static_cast<char>(gene + 32);
			}
			mAppLog.AddLog("Generation %d: %s\n", mGeneticAlgorithm.Getgeneration(), bestStr.c_str());
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

	if (ImGui::Button("Run!"))
	{
		mAppLog.AddLog("Initializing new population...\n");

		auto computeFitness = [](auto& genome)
			{
				float fitness = 0.0f;

				// one point per matching character
				for (size_t i = 0; i < kTarget.size(); ++i) {
					if (kTarget[i] == genome.chromosome[i] + 32) { // 32 is our ASCII offset
						fitness += 1.0f;
					}
				}
				return fitness;
			};

		mGeneticAlgorithm.Initialize(100, static_cast<int>(kTarget.size()), kValidGeneValue, 0.45f, 0.1f, computeFitness);
		mInitialized = true;
	}

	mAppLog.Draw("Console");
}
