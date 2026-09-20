#pragma once

#include "Camera.h"
#include "LightTypes.h"
#include "RenderObject.h"
#include "RenderTarget.h"

namespace SAGE::Graphics
{
	class RenderObject;
	class SpotShadowEffectResources;

	class SpotShadowEffect
	{
	public:
		void Initialize(SpotShadowEffectResources* sharedResources, uint32_t depthMapResolution = 1024);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderGroup& renderGroup);
		void Render(const RenderObject& renderObject);
		void DebugUI();

		void SetSpotLight(const SpotLight& spotLight);

		const Camera& GetLightCamera() const { return mLightCamera; }
		const Texture& GetDepthMap() const { return mDepthMapRenderTarget; }

		bool NeedsUpdate() const;
		void MarkClean();
		void Invalidate();

	private:
		Camera mLightCamera;
		const SpotLight* mSpotLight = nullptr;
		SpotShadowEffectResources* mSharedResources = nullptr;

		RenderTarget mDepthMapRenderTarget;

		// ---------------------------------------- Temp baking ----------------------------------------
		bool mIsDirty = true; // Starts true so the first frame always renders
	};
}