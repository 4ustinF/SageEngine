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

	SAGE::Graphics::Mesh mMesh;
	SAGE::Graphics::RenderObject mEarthRenderObject;

	float mFPS = 0.0f;
	bool mShowNormals = false;
};