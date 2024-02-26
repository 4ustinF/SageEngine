#include <SAGE/Inc/SAGE.h>
#include <ImGui/Inc/AppLog.h>

class GameState : public SAGE::AppState
{
public:
	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	SAGE::GameWorld mGameWorld;
	AppLog mAppLog;

	void TrainModel();

	SAGE::ML::Dataset mDataset;
	SAGE::ML::LogisticRegression mLogReg;
	std::vector<float> mError;
	int mSampleCount = 1000;
	float b0Value = 0.0f;
	float b1Value = 1.0f;
	float b2Value = 1.0f;
	float mMinX = 0.0f;
	float mMaxX = 100.0f;
	float mNoise = 10.0f;
	int mEpoch = 10000;
	float mLearningRate = 0.00055f; 
	bool mFindLowestError = false;
	float mLowestError = 1.0f;

};