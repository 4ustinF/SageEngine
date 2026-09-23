#pragma once

#include "Service.h"

namespace SAGE
{
	class AnimatorComponent;
	class ModelComponent;
	class TransformComponent;
	class SpotlightComponent;
	class Graphics::SpotShadowEffectResources;

	class CameraService;
	class TerrainService;

	enum SkyBoxType
	{
		None,
		CubeMap,
		CrossCubeMap,
		Dome
	};

	class RenderService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::Render)

		void Initialize() override;
		void Terminate() override;

		void Update(float deltaTime) override;

		void Render() override;
		void DebugUI() override;

		void LoadCubeMapSkyBox(const std::vector<const char*>& fileNames, float size = 1000.0f);
		void LoadCrossCubeMapSkyBox(const char* fileName, float size = 1000.0f);
		void LoadSkyDome(const char* fileName, int divisions = 256, float radius = 1000.0f);

		void SetSkyBoxPos(Math::Vector3 position);

		void SetShadowFocus(const Math::Vector3& focusPosition);
		void SetSampleFilter(Graphics::Sampler::Filter sampleFilter) { mSampleFilter = sampleFilter; }

		Graphics::DirectionalLight& GetDirectionalLight() { return mDirectionalLight; }
		const Graphics::DirectionalLight& GetDirectionalLight() const { return mDirectionalLight; }

		// TODO: Remove
		Graphics::ShadowEffect& GetShadowEffect() { return mShadowEffect; }
		Graphics::StandardEffect& GetStandardEffect() { return mStandardEffect; }

	private:
		friend class ModelComponent;
		Graphics::RenderGroup* Register(const ModelComponent* modelComponent, bool isBasic = false);
		void Unregister(const ModelComponent* modelComponent, bool isBasic = false);

		friend class MeshRendererComponent;
		void RegisterMeshRenderer(MeshRendererComponent* meshRendererComponent, bool isBasic = false);
		void UnregisterMeshRenderer(MeshRendererComponent* meshRendererComponent, bool isBasic = false);

		friend class SpotlightComponent;
		bool RegisterSpotLight(SpotlightComponent* spotlightComponent);
		void UnregisterSpotLight(SpotlightComponent* spotlightComponent);

		struct Entry
		{
			const AnimatorComponent* animatorComponent = nullptr;
			const ModelComponent* modelComponent = nullptr;
			const TransformComponent* transformComponent = nullptr;
			Graphics::RenderGroup renderGroup;
		};

		std::vector<Entry> mRenderEntries;
		std::vector<Entry> mBasicRenderEntries;

		std::vector<MeshRendererComponent*> mMeshRendererEntrys; // TODO: Add tiling support to this.
		std::vector<MeshRendererComponent*> mBasicMeshRendererEntrys;


		const CameraService* mCameraService = nullptr;
		const TerrainService* mTerrainService = nullptr;

		// TODO - turn these into components
		Graphics::DirectionalLight mDirectionalLight;

		std::vector<Graphics::SpotLight*> mSpotLights;
		std::vector<SpotlightComponent*> mSpotlightComponents;

		Graphics::SpotShadowEffectResources mSpotShadowEffectResources;

		Graphics::StandardEffect mStandardEffect;
		Graphics::TexturingEffect mSkyBoxEffect;
		Graphics::TexturingEffect mTexturingEffect;
		Graphics::TerrainEffect mTerrainEffect;
		Graphics::ShadowEffect mShadowEffect;

		void RenderSkyBox();
		Graphics::RenderObject mSkyBox;
		Graphics::RenderGroup mNewSkyBox;
		SkyBoxType mSkyBoxType = SkyBoxType::None;
		const float mSkyBoxDefaultSize = 1000.0f;

		Graphics::Sampler::Filter mSampleFilter = Graphics::Sampler::Filter::Linear;

		// FPS
		int mFPS = 0;
		static constexpr int mMaxSampleCount = 150; // max samples you'll ever want
		int mSamepleCount = mMaxSampleCount;
		float mFPSHistory[mMaxSampleCount] = {};
		int mFPSHistoryOffset = 0;
		int mFPSHistorySize = 0; // how many valid samples we have so far
		float mFPSGraphMax = 200.0f;

		// Debug
		int mItemRenderCount = 0;
		int mItemRenderShadowCount = 0;
		void ExtractFrustumPlanes(const Math::Matrix4& vp, Math::Plane outPlanes[6]);

		// TODO: Move out
		Graphics::RenderObject mScreenQuad;
		Graphics::RenderTarget mBaseRenderTarget;
		Graphics::RenderTarget mBloomRenderTarget;
		Graphics::PostProcessingEffect mPostProccessingEffect;
		Graphics::GaussianBlurEffect mGaussianBlurEffect;

	};
}