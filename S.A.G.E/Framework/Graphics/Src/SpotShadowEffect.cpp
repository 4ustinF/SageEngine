#include "Precompiled.h"
#include "SpotShadowEffect.h"

#include "AnimationUtil.h"
#include "Animator.h"
#include "Camera.h"

#include "SpotShadowEffectResources.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

void SpotShadowEffect::Initialize(SpotShadowEffectResources* sharedResources, bool canCastShadow, uint32_t depthMapResolution)
{
	ASSERT(sharedResources != nullptr, "SpotShadowEffect -- sharedResources cannot be null");
	mSharedResources = sharedResources;

	if (canCastShadow)
	{
		EnableDepthMap(depthMapResolution, true);
	}
}

void SpotShadowEffect::Terminate()
{
	mStaticMap.Terminate();
	ReleaseDynamicMap();
	mSpotLight = nullptr;
	mSharedResources = nullptr;
}

void SpotShadowEffect::BeginStatic()
{
	BindSharedState();
	if (bEnableDepthMap)
	{
		mStaticMap.BeginRender();
	}
}

void SpotShadowEffect::EndStatic()
{
	if (bEnableDepthMap)
	{
		mStaticMap.EndRender();
	}
}

void SpotShadowEffect::BeginDynamic()   // RealTime only
{
	BindSharedState();
	if (!bEnableDepthMap)
	{
		return;
	}

	EnsureDynamicMap();
	mDynamicMap.CopyFrom(mStaticMap);
	mDynamicMap.BeginRender(Colors::Black, false);   // no clear, keep the copied statics
}

void SpotShadowEffect::EndDynamic()
{
	if (bEnableDepthMap)
	{
		mDynamicMap.EndRender();
	}
}

void SpotShadowEffect::Render(const RenderGroup& renderGroup, const std::unordered_map<const void*, CachedBoneTransforms>* boneCache)
{
	//for (auto& renderObject : renderGroup) 
	//{
	//	Render(renderObject);
	//}

	for (auto& renderObject : renderGroup)
	{
		const CachedBoneTransforms* cached = nullptr;
		if (boneCache != nullptr)
		{
			auto it = boneCache->find(&renderObject);
			if (it != boneCache->end()) {
				cached = &it->second;
			}
		}
		Render(renderObject, cached);
	}
}

void SpotShadowEffect::Render(const RenderObject& renderObject, const CachedBoneTransforms* cachedBones)
{
	if (!renderObject.canCastShadows)
	{
		return;
	}

	const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mLightCamera.GetViewMatrix();
	const auto& proj = mLightCamera.GetProjectionMatrix();

	SpotShadowTransformData transformData;
	transformData.wvp = Math::Transpose(matWorld * view * proj);

	SpotShadowSettingsData settingsData;
	if (cachedBones != nullptr && cachedBones->useSkinning)
	{
		// Cache hit — just copy already-computed matrices, no recomputation at all.
		SpotShadowBoneTransformData boneTransformData;
		const size_t boneCount = cachedBones->matrices.size();
		for (size_t i = 0; i < boneCount && i < SpotShadowBoneTransformData::MaxBoneCount; ++i) {
			boneTransformData.boneTransforms[i] = cachedBones->matrices[i];
		}
		mSharedResources->boneTransformBuffer.Update(boneTransformData);
		settingsData.useSkinning = 1;
	}
	//else if (renderObject.animator) // Fallback
	//{
	//	SpotShadowBoneTransformData boneTransformData;

	//	std::vector<Math::Matrix4> boneTransforms;
	//	AnimationUtil::ComputeBoneTransforms(*renderObject.skeleton, boneTransforms, [animator = renderObject.animator](const Bone* bone) {return animator->GetTransform(bone); });
	//	AnimationUtil::ApplyBoneOffset(*renderObject.skeleton, boneTransforms);

	//	const size_t boneCount = renderObject.skeleton->bones.size();
	//	for (size_t i = 0; i < boneCount && i < SpotShadowBoneTransformData::MaxBoneCount; ++i) {
	//		boneTransformData.boneTransforms[i] = Math::Transpose(boneTransforms[i]);
	//	}

	//	mSharedResources->boneTransformBuffer.Update(boneTransformData);
	//	settingsData.useSkinning = 1;
	//}
	//else if (renderObject.skeleton) // Fallback
	//{
	//	SpotShadowBoneTransformData boneTransformData;

	//	std::vector<Math::Matrix4> boneTransforms;
	//	AnimationUtil::ComputeBoneTransforms(*renderObject.skeleton, boneTransforms, [](const Bone* bone) {return bone->toParentTransform; });
	//	AnimationUtil::ApplyBoneOffset(*renderObject.skeleton, boneTransforms);

	//	const size_t boneCount = renderObject.skeleton->bones.size();
	//	for (size_t i = 0; i < boneCount && i < SpotShadowBoneTransformData::MaxBoneCount; ++i) {
	//		boneTransformData.boneTransforms[i] = Math::Transpose(boneTransforms[i]);
	//	}

	//	mSharedResources->boneTransformBuffer.Update(boneTransformData);
	//	settingsData.useSkinning = 1;
	//}

	mSharedResources->transformBuffer.Update(transformData);
	mSharedResources->settingsBuffer.Update(settingsData);

	renderObject.meshBuffer.Render();
}

void SpotShadowEffect::SetSpotLight(const SpotLight& spotLight)
{
	const bool same = mHasCachedLight
		&& mCachedPos == spotLight.position
		&& mCachedDir == spotLight.direction
		&& mCachedRange == spotLight.range
		&& mCachedOuterCone == spotLight.outerConeAngle;

	if (same)
	{
		return;
	}

	mCachedPos = spotLight.position;
	mCachedDir = spotLight.direction;
	mCachedRange = spotLight.range;
	mCachedOuterCone = spotLight.outerConeAngle;
	mHasCachedLight = true;

	mLightCamera.SetPosition(spotLight.position);
	mLightCamera.SetDirection(spotLight.direction);
	mLightCamera.SetFarPlane(spotLight.range);
	mLightCamera.SetFov(spotLight.outerConeAngle * 2.2f);
	InvalidateStatic();
}

void SpotShadowEffect::SetLightMode(LightMode newMode)
{
	if (newMode == mLightMode) 
	{ 
		return; 
	}

	const bool wasRealTime = (mLightMode == LightMode::RealTime);
	mLightMode = newMode;

	if (bEnableDepthMap) 
	{ 
		if (wasRealTime)
		{
			ReleaseDynamicMap();        // static map was never touched by dynamics, nothing to clean
		}
		else if (newMode == LightMode::RealTime)
		{
			EnsureDynamicMap();
			mDynamicDirty = true;       // first RealTime frame must build the composite
		}
	}
}

void SpotShadowEffect::EnableDepthMap(uint32_t res, bool force)
{
	if (!force && bEnableDepthMap)
	{
		return;
	}

	if (bEnableDepthMap)            // forced re-init, free what exists first
	{
		mStaticMap.Terminate();
		ReleaseDynamicMap();
	}

	mResolution = res;
	mLightCamera.SetMode(Camera::ProjectionMode::Perspective);
	mLightCamera.SetAspectRatio(1.0f);
	mLightCamera.SetNearPlane(0.5f);

	mStaticMap.Initialize(res, res, Texture::Format::RGBA_U32);
	bEnableDepthMap = true;
	if (mLightMode == LightMode::RealTime) 
	{ 
		EnsureDynamicMap();
	}
	InvalidateStatic();
}

void SpotShadowEffect::DisableDepthMap()
{
	if (!bEnableDepthMap) 
	{ 
		return;
	}

	bEnableDepthMap = false;
	mStaticMap.Terminate();
	ReleaseDynamicMap();
	InvalidateStatic();
}

void SpotShadowEffect::MarkStaticClean() 
{
	mStaticDirty = false; 
}

void SpotShadowEffect::MarkDynamicClean()
{
	if (mLightMode != LightMode::RealTime) 
	{ 
		mDynamicDirty = false; 
	}
}

void SpotShadowEffect::Invalidate()
{
	InvalidateStatic();
}

void SpotShadowEffect::InvalidateStatic()
{
	mStaticDirty = true;
	mDynamicDirty = true;
}

void SpotShadowEffect::InvalidateDynamic()
{
	mDynamicDirty = true;
}

void SpotShadowEffect::BindSharedState()
{
	mSharedResources->vertexShader.Bind();
	mSharedResources->pixelShader.Bind();
	mSharedResources->transformBuffer.BindVS(0);
	mSharedResources->boneTransformBuffer.BindVS(1);
	mSharedResources->settingsBuffer.BindVS(2);
}

void SpotShadowEffect::EnsureDynamicMap()
{
	if (mHasDynamicMap || !bEnableDepthMap) 
	{
		return; 
	}

	mDynamicMap.Initialize(mResolution, mResolution, Texture::Format::RGBA_U32);
	mHasDynamicMap = true;
}

void SpotShadowEffect::ReleaseDynamicMap()
{
	if (!mHasDynamicMap) 
	{ 
		return; 
	}

	mDynamicMap.Terminate();
	mHasDynamicMap = false;
}