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
	struct PlanetInfo
	{
		float scale;
		float rotationSpeed;
		float distFromCen;
		float revolutionSpeed;
		float rotation = 0.0f;
		float revolution = 0.0f;
	};
	PlanetInfo InitalizePlanets(float scale, float rotSpeed, float dist, float revSpeed);
	void UpdatePlanet(PlanetInfo& planet, float deltaTime);
	void RenderPlanet(PlanetInfo& planet, SAGE::Graphics::Texture& texture, SAGE::Math::Matrix4 view, SAGE::Math::Matrix4 proj);

	PlanetInfo sunInfo;
	PlanetInfo mercuryInfo;
	PlanetInfo venusInfo;
	PlanetInfo earthInfo;
	PlanetInfo marsInfo;
	PlanetInfo JupiterInfo;
	PlanetInfo SaturnInfo;
	PlanetInfo UranusInfo;
	PlanetInfo NeptuneInfo;

	SAGE::Graphics::Texture mCubeTexture;
	SAGE::Graphics::Texture mSkyBoxTexture;
	SAGE::Graphics::Texture mSkyDomeTexture;

	SAGE::Graphics::Texture mSunTexture;
	SAGE::Graphics::Texture mMercuyTexture;
	SAGE::Graphics::Texture mVenusTexture;
	SAGE::Graphics::Texture mEarthTexture;
	SAGE::Graphics::Texture mMarsTexture;
	SAGE::Graphics::Texture mJupiterTexture;
	SAGE::Graphics::Texture mSaturnTexture;
	SAGE::Graphics::Texture mUranusTexture;
	SAGE::Graphics::Texture mNeptuneTexture;

	SAGE::Graphics::Sampler mSampler;

	SAGE::Graphics::MeshPX mCubeMesh;
	SAGE::Graphics::MeshPX mSkyBoxMesh;
	SAGE::Graphics::MeshPX mSphereMesh;

	SAGE::Graphics::Camera mCamera;
	const float mCameraMoveSpeed = 0.025f;

	SAGE::Graphics::VertexShader mVertexShader;
	SAGE::Graphics::PixelShader mPixelShader;
	SAGE::Graphics::ConstantBuffer mConstantBuffer;

	SAGE::Graphics::MeshBuffer mCubeBuffer;
	SAGE::Graphics::MeshBuffer mSkyBoxBuffer;
	SAGE::Graphics::MeshBuffer mSphereBuffer;

	SAGE::Math::Matrix4 world = SAGE::Math::Matrix4::Identity;
};