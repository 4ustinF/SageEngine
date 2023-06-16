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
	struct TransformData
	{
		SAGE::Math::Matrix4 world;
		SAGE::Math::Matrix4 wvp;
		SAGE::Math::Vector3 viewPosition;
		float bumpWeight = 0.0f;
	};

	struct SettingsData
	{
		int useDiffuseMap = 1;
		int useSpecularMap = 1;
		int useBumpMap = 1;
		int useNormalMap = 1;
	};

	using TransformBuffer = TypedConstantBuffer<TransformData>;
	using MaterialBuffer = TypedConstantBuffer<Material>;
	using LightBuffer = TypedConstantBuffer<DirectionalLight>;
	using SettingsBuffer = TypedConstantBuffer<SettingsData>;

	TransformBuffer mTransformBuffer;
	MaterialBuffer mMaterialBuffer;
	LightBuffer mLightBuffer;
	SettingsBuffer mSettingsBuffer;

	SAGE::Graphics::Camera mCamera;
	SAGE::Graphics::Mesh mMesh;

	SAGE::Graphics::Sampler mSampler;

	SAGE::Graphics::VertexShader mVertexShader;
	SAGE::Graphics::PixelShader mPixelShader;

	SettingsData mSettingsData;

	DirectionalLight mDirectionalLight;

	float mFPS = 0.0f;
	float mBumpWeight = 0.0f;
	bool mShowNormals = false;
};