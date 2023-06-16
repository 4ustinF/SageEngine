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
	std::vector<SAGE::Graphics::VertexPC> mVertices; //we can get rid of

	SAGE::Graphics::Camera mCamera;
	const float mCameraMoveSpeed = 0.025;

	SAGE::Graphics::VertexShader mVertexShader;
	SAGE::Graphics::PixelShader mPixelShader;
	SAGE::Graphics::ConstantBuffer mConstantBuffer;

	SAGE::Graphics::MeshBuffer mMeshBuffer;

	SAGE::Math::Matrix4 world = SAGE::Math::Matrix4::Identity;
};