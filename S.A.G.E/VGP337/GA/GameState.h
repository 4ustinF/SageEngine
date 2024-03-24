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

	SAGE::ML::GeneticAlgorithm mGeneticAlgorithm;
	bool mInitialized = false;

	float mCrossoverRate = 0.45f;
	float mMutationRate = 0.1f;
};