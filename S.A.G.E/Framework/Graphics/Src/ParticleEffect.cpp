#include "Precompiled.h"
#include "ParticleEffect.h"

#include "Camera.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void ParticleEffect::Initialize(Sampler::Filter sampleFilter)
{
	mVertexShader.Initialize<OldVertex>(L"../../Assets/Shaders/Particles.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/Particles.fx");

	mTransformBuffer.Initialize();
	mSettingsBuffer.Initialize();

	mAlphaBlendState.Initialize(mBlendStateMode);
	mSampler.Initialize(sampleFilter, Sampler::AddressMode::Wrap);
}

void ParticleEffect::Terminate()
{
	mSampler.Terminate();

	mAlphaBlendState.Terminate();

	mSettingsBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void ParticleEffect::Begin()
{
	ASSERT(mCamera != nullptr, "ParticleEffect - No camera set");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mSettingsBuffer.BindVS(1);
	mSettingsBuffer.BindPS(1);

	mSampler.BindVS(0);
	mSampler.BindPS(0);
}

void ParticleEffect::End()
{
}

void ParticleEffect::Render(const RenderGroup& renderGroup)
{
	for (auto& renderObjects : renderGroup) {
		Render(renderObjects);
	}
}

void ParticleEffect::Render(const RenderObject& renderObject)
{
	const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mCamera->GetViewMatrix();
	const auto& proj = mCamera->GetProjectionMatrix();

	TransformData transformData;
	transformData.world = Transpose(matWorld);
	transformData.wvp = Transpose(matWorld * view * proj);
	transformData.viewPosition = mCamera->GetPosition();
	mTransformBuffer.Update(transformData);

	SettingsData settingsData;
	settingsData.particleColor = mParticleColor;
	settingsData.uvOffSet = mUVOffSet;
	mSettingsBuffer.Update(settingsData);

	auto tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);

	mAlphaBlendState.Set();
	renderObject.meshBuffer.Render();
	BlendState::ClearState();
}

void ParticleEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void ParticleEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Particle Effect", ImGuiTreeNodeFlags_CollapsingHeader))
	{

	}
}