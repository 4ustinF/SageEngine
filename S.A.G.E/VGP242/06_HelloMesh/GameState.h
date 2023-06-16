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
	SAGE::Graphics::MeshPC mCubeMesh;
	SAGE::Graphics::MeshPC mPlaneMesh;
	SAGE::Graphics::MeshPC mCylinderMesh;
	SAGE::Graphics::MeshPC mSphereMesh;

	SAGE::Graphics::Camera mCamera;
	const float mCameraMoveSpeed = 0.025f;

	SAGE::Graphics::VertexShader mVertexShader;
	SAGE::Graphics::PixelShader mPixelShader;
	SAGE::Graphics::ConstantBuffer mConstantBuffer;

	SAGE::Graphics::MeshBuffer mCubeMeshBuffer;
	SAGE::Graphics::MeshBuffer mPlaneMeshBuffer;
	SAGE::Graphics::MeshBuffer mCylinderMeshBuffer;
	SAGE::Graphics::MeshBuffer mSphereMeshBuffer;

	SAGE::Math::Matrix4 world = SAGE::Math::Matrix4::Identity;
};