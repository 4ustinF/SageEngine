#include <SAGE/Inc/SAGE.h>

// https://www.rastertek.com/dx11tut27.html
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
	SAGE::Graphics::ReflectionEffect mReflectionEffect;
	SAGE::Graphics::ShadowEffect mShadowEffect;

	SAGE::Graphics::RenderObject mCube;
	SAGE::Graphics::RenderObject mFloor;

	SAGE::Graphics::RenderObject mScreenQuad;
	SAGE::Graphics::RenderTarget mBaseRenderTarget;

	bool useMouseDebugMode = true;
	float mFPS = 0.0f;

};