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

	SAGE::Graphics::SpriteRenderer* mSpriteRenderer = nullptr;
	SAGE::ML::GeneticAlgorithm mGeneticAlgorithm;
	bool mInitialized = false;

	int mPopulationSize = 100;
	float mCrossoverRate = 0.45f;
	float mMutationRate = 0.1f;

	SAGE::Graphics::TextureId mCityTextureID = 0;
	float mHalfScreenWidth = 0.0f;
	float mHalfScreenHeight = 0.0f;
};