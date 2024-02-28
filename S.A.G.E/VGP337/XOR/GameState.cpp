#include "GameState.h"

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

void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/bare.json");

	const std::vector<size_t> topology;
	mNeuralNetwork = new NeuralNetwork({ 2, 2, 1 });

	mInputValues.resize(4);
	mInputValues[0].push_back(0.0f);
	mInputValues[0].push_back(0.0f);
	mInputValues[1].push_back(1.0f);
	mInputValues[1].push_back(0.0f);
	mInputValues[2].push_back(0.0f);
	mInputValues[2].push_back(1.0f);
	mInputValues[3].push_back(1.0f);
	mInputValues[3].push_back(1.0f);

	mTargetValues.resize(4);
	mTargetValues[0].push_back(0.0f);
	mTargetValues[1].push_back(1.0f);
	mTargetValues[2].push_back(1.0f);
	mTargetValues[3].push_back(0.0f);
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
	SafeDelete(mNeuralNetwork);
	mTargetValues.clear();
	mInputValues.clear();
}

void GameState::Update(float deltaTime)
{
	//mGameWorld.Update(deltaTime);

	const auto& inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(KeyCode::SPACE))
	{
		mAppLog.clear();
	}
}

void GameState::Render()
{
	//mGameWorld.Render();
}

void GameState::DebugUI()
{
	//mGameWorld.DebugUI();
	mAppLog.Draw("Console");

	ImGui::SliderInt("X0", &mPredictX0, 0, 1);
	ImGui::SliderInt("X1", &mPredictX1, 0, 1);
	ImGui::SliderInt("Training amount", &mTrainingAmount, 1, 1000);

	if (ImGui::Button("Train Model"))
	{
		TrainModel();
	}

	if (ImGui::Button("Show Result"))
	{
		ShowResult();
	}
}

void GameState::TrainModel()
{
	mAppLog.AddLog("Training model %d times\n", mTrainingAmount);
	int pickedTest = 0;

	for (int i = 0; i < mTrainingAmount; ++i)
	{
		// Pick a test
		pickedTest = Random::UniformInt(0, 3);

		// Feed forward with that data
		mNeuralNetwork->FeedForward(mInputValues[pickedTest]);

		// Back propagate with real values
		mNeuralNetwork->BackPropagate(mTargetValues[pickedTest]);
	}
}

// 0, 0 -> 0
// 0, 1 -> 1
// 1, 0 -> 1
// 1, 1 -> 0

void GameState::ShowResult()
{
	mAppLog.AddLog("Getting Result for prediction %d, %d \n", mPredictX0, mPredictX1);
	const int pickedTest = mPredictX0 + mPredictX0 + mPredictX1;

	// Feed forward with prediction data
	mNeuralNetwork->FeedForward(mInputValues[pickedTest]);

	// Get results from neural network
	const float result = mNeuralNetwork->GetResults()[0];
	mAppLog.AddLog("Result: %f\n", result);

	// Get error
	const float error = result - mTargetValues[pickedTest][0];
	mAppLog.AddLog("Error: %f\n", error);
}
