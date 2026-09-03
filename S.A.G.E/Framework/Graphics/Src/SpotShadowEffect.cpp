#include "Precompiled.h"
#include "SpotShadowEffect.h"

#include "AnimationUtil.h"
#include "Animator.h"
#include "Camera.h"
#include "RenderObject.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

void SpotShadowEffect::Initialize(uint32_t resolution)
{
	mLightCamera.SetMode(Camera::ProjectionMode::Perspective);
	mLightCamera.SetAspectRatio(1.0f); // shadow map is square

	mVertexShader.Initialize<Vertex>(L"../../Assets/Shaders/Shadow.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/Shadow.fx");

	mTransformBuffer.Initialize();
	mBoneTransformBuffer.Initialize();
	mSettingsBuffer.Initialize();

	mDepthMapRenderTarget.Initialize(resolution, resolution, Texture::Format::RGBA_U32);
}

void SpotShadowEffect::Terminate()
{
	mDepthMapRenderTarget.Terminate();

	mSettingsBuffer.Terminate();
	mBoneTransformBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void SpotShadowEffect::Begin()
{
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);
	mBoneTransformBuffer.BindVS(1);
	mSettingsBuffer.BindVS(2);

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
	const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mLightCamera.GetViewMatrix();
	const auto& proj = mLightCamera.GetProjectionMatrix();

	TransformData transformData;
	transformData.wvp = Math::Transpose(matWorld * view * proj);

	SettingsData settingsData;
	if (renderObject.animator)
	{
		BoneTransformData boneTransformData;

		std::vector<Math::Matrix4> boneTransforms;
		AnimationUtil::ComputeBoneTransforms(*renderObject.skeleton, boneTransforms, [animator = renderObject.animator](const Bone* bone) {return animator->GetTransform(bone); });
		AnimationUtil::ApplyBoneOffset(*renderObject.skeleton, boneTransforms);

		const size_t boneCount = renderObject.skeleton->bones.size();
		for (size_t i = 0; i < boneCount && i < BoneTransformData::MaxBoneCount; ++i) {
			boneTransformData.boneTransforms[i] = Math::Transpose(boneTransforms[i]);
		}

		mBoneTransformBuffer.Update(boneTransformData);
		settingsData.useSkinning = 1;
	}
	else if (renderObject.skeleton)
	{
		BoneTransformData boneTransformData;

		std::vector<Math::Matrix4> boneTransforms;
		AnimationUtil::ComputeBoneTransforms(*renderObject.skeleton, boneTransforms, [](const Bone* bone) {return bone->toParentTransform; });
		AnimationUtil::ApplyBoneOffset(*renderObject.skeleton, boneTransforms);

		const size_t boneCount = renderObject.skeleton->bones.size();
		for (size_t i = 0; i < boneCount && i < BoneTransformData::MaxBoneCount; ++i) {
			boneTransformData.boneTransforms[i] = Math::Transpose(boneTransforms[i]);
		}

		mBoneTransformBuffer.Update(boneTransformData);
		settingsData.useSkinning = 1;
	}

	mTransformBuffer.Update(transformData);
	mSettingsBuffer.Update(settingsData);

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
}

void SpotShadowEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Shadow Effect", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Text("Depth Map");
		ImGui::Image(mDepthMapRenderTarget.GetRawData(), { 144, 144 }, { 0, 0 }, { 1, 1 }, {1, 1, 1, 1}, { 1, 1, 1, 1 });
	}
}