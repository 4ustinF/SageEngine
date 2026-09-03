#pragma once

#include "Service.h"

namespace SAGE
{
	class AnimatorComponent;
	class ModelComponent;
	class TransformComponent;

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

		void SetSkyBoxPos(SAGE::Math::Vector3 position);

		void SetShadowFocus(const Math::Vector3& focusPosition);
		void SetSampleFilter(SAGE::Graphics::Sampler::Filter sampleFilter) { mSampleFilter = sampleFilter; }

		SAGE::Graphics::DirectionalLight& GetDirectionalLight() { return mDirectionalLight; }
		const SAGE::Graphics::DirectionalLight& GetDirectionalLight() const { return mDirectionalLight; }

		// TODO: Remove
		SAGE::Graphics::ShadowEffect& GetShadowEffect() { return mShadowEffect; }

	private:
		friend class ModelComponent;
		Graphics::RenderGroup* Register(const ModelComponent* modelComponent, bool isBasic = false);
		void Unregister(const ModelComponent* modelComponent, bool isBasic = false);

		friend class MeshRendererComponent;
		void RegisterMeshRenderer(MeshRendererComponent* meshRendererComponent, bool isBasic = false);
		void UnregisterMeshRenderer(MeshRendererComponent* meshRendererComponent, bool isBasic = false);

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
		SAGE::Graphics::DirectionalLight mDirectionalLight;

		std::array<SAGE::Graphics::SpotLight, Graphics::MaxSpotLights> mSpotLights;
		std::array<SAGE::Graphics::SpotShadowEffect, Graphics::MaxSpotLights> mSpotShadowEffects;
		size_t mActiveSpotLightCount = 0;

		SAGE::Graphics::StandardEffect mStandardEffect;
		SAGE::Graphics::TexturingEffect mSkyBoxEffect;
		SAGE::Graphics::TexturingEffect mTexturingEffect;
		SAGE::Graphics::TerrainEffect mTerrainEffect;
		SAGE::Graphics::ShadowEffect mShadowEffect;

		void RenderSkyBox();
		SAGE::Graphics::RenderObject mSkyBox;
		SAGE::Graphics::RenderGroup mNewSkyBox;
		SkyBoxType mSkyBoxType = SkyBoxType::None;
		const float mSkyBoxDefaultSize = 1000.0f;

		SAGE::Graphics::Sampler::Filter mSampleFilter = SAGE::Graphics::Sampler::Filter::Linear;

		int mFPS = 0;

		// TODO: Move out
		SAGE::Graphics::RenderObject mScreenQuad;
		SAGE::Graphics::RenderTarget mBaseRenderTarget;
		SAGE::Graphics::RenderTarget mBloomRenderTarget;
		SAGE::Graphics::PostProcessingEffect mPostProccessingEffect;
		SAGE::Graphics::GaussianBlurEffect mGaussianBlurEffect;

	};
}