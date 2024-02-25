#include "GameState.h"
#include <ImGui/Inc/ImPlot.h>

using namespace SAGE;
using namespace SAGE::Physics;
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
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);

	const auto& inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(KeyCode::SPACE))
	{
		mAppLog.clear();
	}
}

void GameState::Render()
{
	mGameWorld.Render();
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();
	mAppLog.Draw("Console");

	if (ImGui::Button("Train Model"))
	{
		TrainModel();
	}

	ImGui::DragFloat("Learning Rate", &mLearningRate, 0.01f, 0.000001f, 0.001f, "%.7f");
	ImGui::SliderInt("Epochs", &mEpoch, 100, 10000);

	ImGui::SliderInt("Sample Count", &mSampleCount, 10, 1000);
	ImGui::DragFloat("B0Value", &b0Value, 0.01f, -10.0f, 10.0f, "%.2f");
	ImGui::DragFloat("B1Value", &b1Value, 0.01f, -10.0f, 10.0f, "%.2f");
	ImGui::DragFloat("Min X", &mMinX, 0.01f, 0.0f, 100.0f, "%.2f");
	ImGui::DragFloat("Max X", &mMaxX, 0.01f, 0.0f, 100.0f, "%.2f");
	ImGui::DragFloat("Noise", &mNoise, 0.01f, 0.0f, 100.0f, "%.2f");

	ImGui::Checkbox("Find Lowest Error", &mFindLowestError);
	ImGui::DragFloat("Lowest Error", &mLowestError, 0.01f, 0.0f, 0.001f, "%.7f");

	if (ImPlot::BeginPlot("Linear regression", "x", "y"))
	{
		// Line
		const float startY = mLinReg.Predict(mMinX);
		const float endY = mLinReg.Predict(mMaxX);
		std::vector<ImVec2> line;
		line.push_back({ mMinX, startY });
		line.push_back({ mMaxX, endY });

		ImPlot::PlotScatter("Dataset", mDataset.x0.data(), mDataset.y.data(), static_cast<int>(mDataset.x0.size()));
		ImPlot::PlotLine("Model", line.data(), static_cast<int>(line.size()));
		ImPlot::EndPlot();
	}
}

void GameState::TrainModel()
{
	mError.clear();

	mDataset = Datasets::MakeLinear(mSampleCount, b0Value, b1Value, mMinX, mMaxX, mNoise);
	mLinReg.Reset(b0Value, b1Value, mLearningRate);

	mAppLog.AddLog("Running gradient descent...\n");

	if (mFindLowestError)
	{
		while (true)
		{
			const float err = mLinReg.Fit(mDataset);
			mAppLog.AddLog("B0=%f B1=%f error=%f\n", mLinReg.b0, mLinReg.b1, err);
			mError.push_back(err);
			if (fabs(err) < mLowestError)
			{
				break;
			}
		}
	}
	else
	{
		for (int e = 0; e < mEpoch; ++e)
		{
			const float err = mLinReg.Fit(mDataset);
			mAppLog.AddLog("B0=%f B1=%f error=%f\n", mLinReg.b0, mLinReg.b1, err);
			mError.push_back(err);
		}
	}
}