#include <SAGE/Inc/SAGE.h>

class GameState : public SAGE::AppState
{
public:
	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	SAGE::Graphics::Camera mCamera;
	SAGE::Graphics::DirectionalLight mDirectionalLight;

	SAGE::Graphics::StandardEffect mStandardEffect;
	SAGE::Graphics::ShadowEffect mShadowEffect;

	SAGE::Graphics::RenderObject mDuckRenderObject;
	SAGE::Graphics::RenderObject mGroundRenderObject;

	float mFPS = 0.0f;
	float mBallHeight= 5.0f;

	float mRunningTime = 0.0f;
	bool mPause = false;
};