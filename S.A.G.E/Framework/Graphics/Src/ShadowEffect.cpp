#include "Precompiled.h"
#include "ShadowEffect.h"

#include "AnimationUtil.h"
#include "Animator.h"
#include "Camera.h"
#include "RenderObject.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void ShadowEffect::Initialize()
{
	mLightCamera.SetMode(Camera::ProjectionMode::Orthographic);

	mVertexShader.Initialize<Vertex>(L"../../Assets/Shaders/Shadow.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/Shadow.fx");

	mTransformBuffer.Initialize();
	mBoneTransformBuffer.Initialize();
	mSettingsBuffer.Initialize();

	constexpr uint32_t depthMapResolution = 4096;
	mDepthMapRenderTarget.Initialize(depthMapResolution, depthMapResolution, Texture::Format::RGBA_U32);
}

void ShadowEffect::Terminate()
{
	mDepthMapRenderTarget.Terminate();

	mSettingsBuffer.Terminate();
	mBoneTransformBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void ShadowEffect::Begin()
{
	UpdateLightCamera();

	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);
	mBoneTransformBuffer.BindVS(1);
	mSettingsBuffer.BindVS(2);

	mDepthMapRenderTarget.BeginRender();
}

void ShadowEffect::End()
{
	mDepthMapRenderTarget.EndRender();
}

void ShadowEffect::Render(const RenderObject& renderObject)
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

void ShadowEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void ShadowEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Shadow Effect", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Text("Depth Map");
		ImGui::Image(mDepthMapRenderTarget.GetRawData(), { 144, 144 }, { 0, 0 }, { 1, 1 }, {1, 1, 1, 1}, { 1, 1, 1, 1 });
		ImGui::DragFloat("Size", &mSize, 1.0f, 1.0f, 1000.0f);
	}
}

void ShadowEffect::UpdateLightCamera()
{
	ASSERT(mDirectionalLight != nullptr, "ShadowEffect --- No light set!");

	const Math::Vector3& direction = mDirectionalLight->direction;
	mLightCamera.SetDirection(direction);
	mLightCamera.SetPosition(mFocusPosition - (direction * 1000.0f));
	mLightCamera.SetNearPlane(1.0f);
	mLightCamera.SetFarPlane(1500.0f);
	mLightCamera.SetSize(mSize, mSize);
}