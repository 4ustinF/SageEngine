#pragma once

#include "Service.h"

namespace SAGE
{
	class AnimatorComponent;
	class ModelComponent;
	class TransformComponent;

	class CameraService;
	class TerrainService;

	class RenderService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::Render)

		void Initialize() override;
		void Terminate() override;

		void Update(float deltaTime) override;

		void Render() override;
		void DebugUI() override;

		void LoadSkyDome(const char* fileName); // TODO: Pass size as parameter, default it at 500.0f
		void LoadSkyBox(const char* fileName); // TODO: Pass size as parameter, default it at 500.0f

		void SetSkyDomePos(SAGE::Math::Vector3 position);
		void SetSkyBoxPos(SAGE::Math::Vector3 position);

		void SetShadowFocus(const Math::Vector3& focusPosition);
		void SetSampleFilter(SAGE::Graphics::Sampler::Filter sampleFilter) { mSampleFilter = sampleFilter; }

		SAGE::Graphics::DirectionalLight& GetDirectionalLight() { return mDirectionalLight; }
		const SAGE::Graphics::DirectionalLight& GetDirectionalLight() const { return mDirectionalLight; }

	private:
		friend class ModelComponent;
		void Register(const ModelComponent* modelComponent);
		void Unregister(const ModelComponent* modelComponent);

		struct Entry
		{
			const AnimatorComponent* animatorComponent = nullptr;
			const ModelComponent* modelComponent = nullptr;
			const TransformComponent* transformComponent = nullptr;
			Graphics::RenderGroup renderGroup;
		};

		std::vector<Entry> mRenderEntries;

		const CameraService* mCameraService = nullptr;
		const TerrainService* mTerrainService = nullptr;

		// TODO - turn these into components
		SAGE::Graphics::DirectionalLight mDirectionalLight;

		SAGE::Graphics::StandardEffect mStandardEffect;
		SAGE::Graphics::TexturingEffect mTexturingEffect;
		SAGE::Graphics::TerrainEffect mTerrainEffect;
		SAGE::Graphics::ShadowEffect mShadowEffect;

		SAGE::Graphics::RenderObject mSkyDome;
		SAGE::Graphics::RenderObject mSkyBox;

		SAGE::Graphics::Sampler::Filter mSampleFilter = SAGE::Graphics::Sampler::Filter::Linear;

		float mFPS = 0.0f;
	};
}