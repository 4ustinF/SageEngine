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
	SAGE::Graphics::PostProcessingEffect mPostProccessingEffect;
	SAGE::Graphics::GaussianBlurEffect mGaussianBlurEffect;

	SAGE::Graphics::RenderTarget mBaseRenderTarget;
	SAGE::Graphics::RenderTarget mBloomRenderTarget;

	SAGE::Graphics::RenderObject mSunRenderObject;
	SAGE::Graphics::RenderObject mEarthRenderObject;
	SAGE::Graphics::RenderObject mScreenQuad;


	float mFPS = 0.0f;
	float mEarthOffSet = 8.0f;
	float mEarthRotation = 0.0f;
	float mEarthRevolution = 0.0f;
};