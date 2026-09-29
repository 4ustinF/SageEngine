#pragma once

#include "Camera.h"
#include "LightTypes.h"
#include "RenderObject.h"
#include "RenderTarget.h"

namespace SAGE::Graphics
{
	class RenderObject;
	class SpotShadowEffectResources;

	enum LightMode : uint32_t
	{
		RealTime,
		PseudoBaked,
		Baked,
	};

	class SpotShadowEffect
	{
	public:
		void Initialize(SpotShadowEffectResources* sharedResources, bool canCastShadow, uint32_t depthMapResolution = 1024);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderGroup& renderGroup);
		void Render(const RenderObject& renderObject);

		const Camera& GetLightCamera() const { return mLightCamera; }
		const Texture& GetDepthMap() const { return mDepthMapRenderTarget; }
		LightMode GetLightMode() const { return mLightMode; }

		void SetSpotLight(const SpotLight& spotLight);
		void SetLightMode(LightMode ligthMode);

		void EnableDepthMap(uint32_t depthMapResolution = 1024, bool force = false);
		void DisableDepthMap();

		bool NeedsUpdate() const;
		void MarkClean();
		void Invalidate();

	private:
		Camera mLightCamera;
		const SpotLight* mSpotLight = nullptr;
		SpotShadowEffectResources* mSharedResources = nullptr;

		RenderTarget mDepthMapRenderTarget;
		bool bEnableDepthMap = true;

		// ---------------------------------------- Temp baking ----------------------------------------
		bool mIsDirty = true; // Starts true so the first frame always renders
		LightMode mLightMode = LightMode::PseudoBaked;
	};
}