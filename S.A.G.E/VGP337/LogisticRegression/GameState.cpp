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
	/*mAppLog.Draw("Console");

	if (ImGui::Button("Train Model"))
	{
		TrainModel();
	}

	ImGui::DragFloat("Learning Rate", &mLearningRate, 0.01f, 0.000001f, 0.001f, "%.7f");
	ImGui::SliderInt("Epochs", &mEpoch, 100, 10000);

	ImGui::SliderInt("Sample Count", &mSampleCount, 10, 1000);
	ImGui::DragFloat("B0Value", &b0Value, 0.01f, -10.0f, 10.0f, "%.2f");
	ImGui::DragFloat("B1Value", &b1Value, 0.01f, -10.0f, 10.0f, "%.2f");
	ImGui::DragFloat("B2Value", &b2Value, 0.01f, -10.0f, 10.0f, "%.2f");
	ImGui::DragFloat("Min X", &mMinX, 0.01f, 0.0f, 100.0f, "%.2f");
	ImGui::DragFloat("Max X", &mMaxX, 0.01f, 0.0f, 100.0f, "%.2f");
	ImGui::DragFloat("Noise", &mNoise, 0.01f, 0.0f, 100.0f, "%.2f");

	ImGui::Checkbox("Find Lowest Error", &mFindLowestError);
	ImGui::DragFloat("Lowest Error", &mLowestError, 0.01f, 0.0f, 0.001f, "%.7f");

	if (ImPlot::BeginPlot("Logistic regression", "x0", "x1"))
	{
		std::vector<ImVec2> modelLine;
		const int count = static_cast<int>(std::size(x1));
		modelLine.push_back({0.0f, b0Value / -b2Value });
		modelLine.push_back({10.0f, (b0Value + (b1Value * 10)) / -b2Value });
		ImPlot::PlotLine("Model", modelLine.data(), static_cast<int>(modelLine.size());

		std::vector<float> ax, ay, bx, by;
		for (int i = 0; i < count; ++i)
		{
			const float p = b0Value + b1Value * x1[i] + b2Value * x2[i];
			if (p >= 0)
			{
				ax.push_back(x1[i]);
				ay.push_back(x2[i]);
			}
			else
			{
				bx.push_back(x1[i]);
				by.push_back(x2[i]);
			}
		}

		ImPlot::PlotScatter("Ones", ax.data(), ay.data(), static_cast<int>(ax.size()));
		ImPlot::PlotScatter("Zeros", bx.data(), by.data(), static_cast<int>(bx.size()));
		ImPlot::EndPlot();
	}*/
}

void GameState::TrainModel()
{
	mError.clear();

	mDataset = Datasets::MakeLogistic(mSampleCount, b0Value, b1Value, b2Value, mMinX, mMaxX, mNoise);
	mLogReg.Reset(b0Value, b1Value, b2Value, mLearningRate);

	mAppLog.AddLog("Running gradient descent...\n");

	if (mFindLowestError)
	{
		while (true)
		{
			const float err = mLogReg.Fit(mDataset);
			mAppLog.AddLog("B0=%f B1=%f B2=%f error=%f\n", mLogReg.b0, mLogReg.b1, mLogReg.b2, err);
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
			const float err = mLogReg.Fit(mDataset);
			mAppLog.AddLog("B0=%f B1=%f B2=%f error=%f\n", mLogReg.b0, mLogReg.b1, mLogReg.b2, err);
			mError.push_back(err);
		}
	}
}