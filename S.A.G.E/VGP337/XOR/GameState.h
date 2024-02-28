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
	void ShowResult();
	SAGE::ML::NeuralNetwork* mNeuralNetwork;
	std::vector<std::vector<float>> mInputValues;
	std::vector<std::vector<float>> mTargetValues;
	int mTrainingAmount = 1000;
	int mPredictX0 = 0;
	int mPredictX1 = 0;

};