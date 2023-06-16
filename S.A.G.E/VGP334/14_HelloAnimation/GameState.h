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

	SAGE::Graphics::RenderObject mBallRenderObject;
	SAGE::Graphics::RenderObject mGroundRenderObject;

	SAGE::Graphics::Animation mAnimation;

	float mFPS = 0.0f;
	float mAnimationTime = 0.0f;
};