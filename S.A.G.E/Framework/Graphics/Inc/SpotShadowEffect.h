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

		void BeginStatic();
		void EndStatic();
		void BeginDynamic();   // Copies static -> dynamic, then renders on top without clearing
		void EndDynamic();

		void Render(const RenderGroup& renderGroup, const std::unordered_map<const void*, CachedBoneTransforms>* boneCache = nullptr);
		void Render(const RenderObject& renderObject, const CachedBoneTransforms* cachedBones = nullptr);

		Camera& GetLightCamera() { return mLightCamera; }
		const Camera& GetLightCamera() const { return mLightCamera; }
		const Texture& GetStaticDepthMap() const { return mStaticMap; }
		//const Texture& GetDepthMap() const { return mShadowMap; } // the map the lighting shader samples

		const Texture& GetDepthMap() const
		{
			return (mLightMode == LightMode::RealTime && mHasDynamicMap) ? mDynamicMap : mStaticMap;
		}

		LightMode GetLightMode() const { return mLightMode; }

		void SetSpotLight(const SpotLight& spotLight);
		void SetLightMode(LightMode ligthMode);

		void EnableDepthMap(uint32_t depthMapResolution = 1024, bool force = false);
		void DisableDepthMap();

		bool NeedsStaticUpdate() const { return mStaticDirty; }
		bool NeedsDynamicUpdate() const { return mLightMode == LightMode::RealTime && mDynamicDirty; }

		void MarkStaticClean();
		void MarkDynamicClean();

		void Invalidate();
		void InvalidateStatic();   // Also invalidates dynamic, since it's built from static
		void InvalidateDynamic();

	private:
		Camera mLightCamera;
		const SpotLight* mSpotLight = nullptr;
		SpotShadowEffectResources* mSharedResources = nullptr;

		void BindSharedState();
		void EnsureDynamicMap();
		void ReleaseDynamicMap();
		RenderTarget mStaticMap;    // statics only, always allocated while depth map is enabled
		RenderTarget mDynamicMap;   // static + dynamics, allocated only in RealTime
		bool mStaticDirty = true;
		bool mDynamicDirty = true;
		bool bEnableDepthMap = true;
		bool mHasDynamicMap = false;
		uint32_t mResolution = 0;

		// ---------------------------------------- Temp baking ----------------------------------------
		bool mIsDirty = true; // Starts true so the first frame always renders
		LightMode mLightMode = LightMode::RealTime;

		// Cached light params so SetSpotLight only invalidates on real change
		bool mHasCachedLight = false;
		Math::Vector3 mCachedPos = Math::Vector3::Zero;
		Math::Vector3 mCachedDir = Math::Vector3::Zero;
		float mCachedRange = 0.0f;
		float mCachedOuterCone = 0.0f;
	};
}