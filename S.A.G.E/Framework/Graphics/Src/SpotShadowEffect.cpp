#include "Precompiled.h"
#include "SpotShadowEffect.h"

#include "AnimationUtil.h"
#include "Animator.h"
#include "Camera.h"

#include "SpotShadowEffectResources.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

void SpotShadowEffect::Initialize(SpotShadowEffectResources* sharedResources, uint32_t depthMapResolution)
{
	ASSERT(sharedResources != nullptr, "SpotShadowEffect -- sharedResources cannot be null");
	mSharedResources = sharedResources;

	mLightCamera.SetMode(Camera::ProjectionMode::Perspective);
	mLightCamera.SetAspectRatio(1.0f); // shadow map is square

	mDepthMapRenderTarget.Initialize(depthMapResolution, depthMapResolution, Texture::Format::RGBA_U32);
}

void SpotShadowEffect::Terminate()
{
	mDepthMapRenderTarget.Terminate();

	mSpotLight = nullptr;
	mSharedResources = nullptr;
}

void SpotShadowEffect::Begin()
{
	mSharedResources->vertexShader.Bind();
	mSharedResources->pixelShader.Bind();

	mSharedResources->transformBuffer.BindVS(0);
	mSharedResources->boneTransformBuffer.BindVS(1);
	mSharedResources->settingsBuffer.BindVS(2);

	mDepthMapRenderTarget.BeginRender();
}

void SpotShadowEffect::End()
{
	mDepthMapRenderTarget.EndRender();
}

void SpotShadowEffect::Render(const RenderGroup& renderGroup)
{
	for (auto& renderObjects : renderGroup) 
	{
		Render(renderObjects);
	}
}

void SpotShadowEffect::Render(const RenderObject& renderObject)
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
	if (renderObject.animator)
	{
		SpotShadowBoneTransformData boneTransformData;

		std::vector<Math::Matrix4> boneTransforms;
		AnimationUtil::ComputeBoneTransforms(*renderObject.skeleton, boneTransforms, [animator = renderObject.animator](const Bone* bone) {return animator->GetTransform(bone); });
		AnimationUtil::ApplyBoneOffset(*renderObject.skeleton, boneTransforms);

		const size_t boneCount = renderObject.skeleton->bones.size();
		for (size_t i = 0; i < boneCount && i < SpotShadowBoneTransformData::MaxBoneCount; ++i) {
			boneTransformData.boneTransforms[i] = Math::Transpose(boneTransforms[i]);
		}

		mSharedResources->boneTransformBuffer.Update(boneTransformData);
		settingsData.useSkinning = 1;
	}
	else if (renderObject.skeleton)
	{
		SpotShadowBoneTransformData boneTransformData;

		std::vector<Math::Matrix4> boneTransforms;
		AnimationUtil::ComputeBoneTransforms(*renderObject.skeleton, boneTransforms, [](const Bone* bone) {return bone->toParentTransform; });
		AnimationUtil::ApplyBoneOffset(*renderObject.skeleton, boneTransforms);

		const size_t boneCount = renderObject.skeleton->bones.size();
		for (size_t i = 0; i < boneCount && i < SpotShadowBoneTransformData::MaxBoneCount; ++i) {
			boneTransformData.boneTransforms[i] = Math::Transpose(boneTransforms[i]);
		}

		mSharedResources->boneTransformBuffer.Update(boneTransformData);
		settingsData.useSkinning = 1;
	}

	mSharedResources->transformBuffer.Update(transformData);
	mSharedResources->settingsBuffer.Update(settingsData);

	renderObject.meshBuffer.Render();
}

void SpotShadowEffect::SetSpotLight(const SpotLight& spotLight)
{
	mLightCamera.SetPosition(spotLight.position);
	mLightCamera.SetDirection(spotLight.direction);
	mLightCamera.SetNearPlane(0.5f);
	mLightCamera.SetFarPlane(spotLight.range);
	// FOV needs a little headroom past the outer cone or edges clip out of the shadow frustum
	mLightCamera.SetFov(Math::Clamp(spotLight.outerConeAngle * 2.2f, 10.0f * Math::Constants::DegToRad, 170.0f * Math::Constants::DegToRad));
	Invalidate();
}

bool SpotShadowEffect::NeedsUpdate() const
{
	return mIsDirty;
}

void SpotShadowEffect::MarkClean()
{
	mIsDirty = false;
}

void SpotShadowEffect::Invalidate() 
{ 
	mIsDirty = true; 
}