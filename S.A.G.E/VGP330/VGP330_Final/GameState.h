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
	SAGE::Graphics::HologramEffect mHologramEffect;
	SAGE::Graphics::CrossHatchEffect mCrossHatchEffect;

	SAGE::Graphics::RenderObject mGroundRenderObject;

	//SAGE::Graphics::RenderObject mMasterChiefBody;
	//SAGE::Graphics::RenderObject mMasterChiefHead;
	SAGE::Graphics::Model mMasterChiefModel;
	SAGE::Graphics::RenderGroup mMasterChief;

	SAGE::Graphics::RenderObject mCortanaBody;
	SAGE::Graphics::RenderObject mCortanaHead;
	SAGE::Graphics::RenderObject mCortanaEyes;
	SAGE::Graphics::RenderObject mCortanaHair;

	SAGE::Graphics::Sampler mSampler;
	SAGE::Graphics::VertexShader mVertexShader;
	SAGE::Graphics::PixelShader mPixelShader;
	SAGE::Graphics::ConstantBuffer mConstantBuffer;

	SAGE::Graphics::MeshPX mSkyBoxMesh;
	SAGE::Graphics::Texture mSkyBoxTexture;
	SAGE::Graphics::MeshBuffer mSkyBoxBuffer;

	SAGE::Graphics::RenderTarget mBaseRenderTarget;	
	SAGE::Graphics::RenderTarget mBloomRenderTarget;
	SAGE::Graphics::GaussianBlurEffect mGaussianBlurEffect;
	SAGE::Graphics::PostProcessingEffect mPostProccessingEffect;
	SAGE::Graphics::RenderObject mScreenQuad;

	float mFPS = 0.0f;
	float mBallHeight= 5.0f;

	float mRunningTime = 0.0f;
	bool mPause = true;
};