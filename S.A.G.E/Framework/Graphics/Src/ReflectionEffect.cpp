#include "Precompiled.h"
#include "ReflectionEffect.h"

#include "Camera.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void ReflectionEffect::Initialize(Sampler::Filter sampleFilter)
{
	mVertexShader.Initialize<OldVertex>(L"../../Assets/Shaders/Reflection.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/Reflection.fx");

	mTransformBuffer.Initialize();
	mLightBuffer.Initialize();
	mMaterialBuffer.Initialize();
	mReflectionBuffer.Initialize();

	mAlphaBlendState.Initialize(mBlendStateMode);
	mSampler.Initialize(sampleFilter, Sampler::AddressMode::Wrap);
}

void ReflectionEffect::Terminate()
{
	mSampler.Terminate();

	mAlphaBlendState.Terminate();

	mReflectionBuffer.Terminate();
	mMaterialBuffer.Terminate();
	mLightBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void ReflectionEffect::Begin()
{
	ASSERT(mCamera != nullptr, "ReflectionEffect - No camera set");
	ASSERT(mDirectionalLight != nullptr, "ReflectionEffect - No light set");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mLightBuffer.BindVS(1);
	mLightBuffer.BindPS(1);

	mMaterialBuffer.BindPS(2);

	mReflectionBuffer.BindPS(3);

	mSampler.BindVS(0);
	mSampler.BindPS(0);
}

void ReflectionEffect::End()
{
}

void ReflectionEffect::Render(const RenderGroup& renderGroup)
{
	for (auto& renderObjects : renderGroup) {
		Render(renderObjects);
	}
}

void ReflectionEffect::Render(const RenderObject& renderObject)
{
	const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mCamera->GetViewMatrix();
	const auto& proj = mCamera->GetProjectionMatrix();

	TransformData transformData;
	transformData.world = Transpose(matWorld);
	transformData.wvp[0] = Transpose(matWorld * view * proj);
	transformData.viewPosition = mCamera->GetPosition();
	mTransformBuffer.Update(transformData);

	mLightBuffer.Update(*mDirectionalLight);
	mMaterialBuffer.Update(renderObject.material);

	ReflectionData reflectionData;
	reflectionData.reflectionMatrix = Math::Matrix4::Identity;
	mReflectionBuffer.Update(reflectionData);

	auto tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);

	mAlphaBlendState.Set();
	renderObject.meshBuffer.Render();
	BlendState::ClearState();
}

void ReflectionEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void ReflectionEffect::SetLightCamera(const Camera& camera)
{
	mLightCamera = &camera;
}

void ReflectionEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void ReflectionEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Reflection Effect", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		
	}
}