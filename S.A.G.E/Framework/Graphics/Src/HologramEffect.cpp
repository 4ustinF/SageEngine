#include "Precompiled.h"
#include "HologramEffect.h"

#include "Camera.h"
#include "RenderObject.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void HologramEffect::Initialize()
{
	mVertexShader.Initialize<Vertex>(L"../../Assets/Shaders/HologramShader.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/HologramShader.fx");

	mTransformBuffer.Initialize();
	mSettingsBuffer.Initialize();
	mHologramBuffer.Initialize();

	mAlphaBlendState.Initialize(BlendState::Mode::AlphaBlend);

	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void HologramEffect::Terminate()
{
	mSampler.Terminate();

	mAlphaBlendState.Terminate();

	mHologramBuffer.Terminate();
	mSettingsBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void HologramEffect::Begin()
{
	ASSERT(mCamera != nullptr, "HologramEffect - No camera set");
	ASSERT(mDirectionalLight != nullptr, "HologramEffect - No light set");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mSettingsBuffer.BindVS(1);
	mSettingsBuffer.BindPS(1);

	mHologramBuffer.BindPS(2);
	mHologramBuffer.BindVS(2);

	mSampler.BindPS(0);
}

void HologramEffect::End()
{

}

void HologramEffect::Render(const RenderObject& renderObject)
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
	settingsData.useDiffuseMap = mSettingsData.useDiffuseMap;
	settingsData.useBars = mSettingsData.useBars;
	settingsData.useAlpha = mSettingsData.useAlpha;
	settingsData.shouldFlicker = mSettingsData.shouldFlicker;
	settingsData.hasRim = mSettingsData.hasRim;
	settingsData.hasGlow = mSettingsData.hasGlow;
	settingsData.shouldGlitch = mSettingsData.shouldGlitch;
	mSettingsBuffer.Update(settingsData);

	HologramData hologramData;
	hologramData.deltaTime = SAGE::Core::TimeUtil::GetTime();
	hologramData.barSpeed = mBarSpeed;
	hologramData.barDistance = mBarDistance;
	hologramData.mAlpha = mAlpha;
	hologramData.flickerSpeed = mFlickerSpeed;
	hologramData.rimPower = mRimPower;
	hologramData.glowDistance = mGlowDistance;
	hologramData.glowSpeed = mGlowSpeed;
	hologramData.rimColor = mRimColor;
	hologramData.mainColor = mMainColor;
	hologramData.glitchIntensity = mGlitchIntensity;
	hologramData.glitchSpeed = mGlitchSpeed;
	mHologramBuffer.Update(hologramData);

	auto tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);

	mAlphaBlendState.Set();
	renderObject.meshBuffer.Render();
	BlendState::ClearState();
}

void HologramEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void HologramEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void HologramEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Hologram Effect", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Separator();

		bool useBars = (mSettingsData.useBars == 1);
		ImGui::Checkbox("Use Bars##HologramEffect", &useBars);
		mSettingsData.useBars = useBars ? 1 : 0;

		bool useDiffuseMap = (mSettingsData.useDiffuseMap == 1);
		ImGui::Checkbox("Use Diffuse Map##HologramEffect", &useDiffuseMap);
		mSettingsData.useDiffuseMap = useDiffuseMap ? 1 : 0;

		bool useAlpha = (mSettingsData.useAlpha == 1);
		ImGui::Checkbox("Use Alpha##HologramEffect", &useAlpha);
		mSettingsData.useAlpha = useAlpha ? 1 : 0;

		bool shouldFlicker = (mSettingsData.shouldFlicker == 1);
		ImGui::Checkbox("Should Flicker##HologramEffect", &shouldFlicker);
		mSettingsData.shouldFlicker = shouldFlicker ? 1 : 0;

		bool hasRim = (mSettingsData.hasRim == 1);
		ImGui::Checkbox("Has Rim##HologramEffect", &hasRim);
		mSettingsData.hasRim = hasRim ? 1 : 0;

		bool hasGlow = (mSettingsData.hasGlow == 1);
		ImGui::Checkbox("Has Glow##HologramEffect", &hasGlow);
		mSettingsData.hasGlow = hasGlow ? 1 : 0;

		bool shouldGlitch = (mSettingsData.shouldGlitch == 1);
		ImGui::Checkbox("Should Glitch##HologramEffect", &shouldGlitch);
		mSettingsData.shouldGlitch = shouldGlitch ? 1 : 0;

		ImGui::DragFloat("Bar Speed##HologramEffect", &mBarSpeed, 0.01f, 0.0f, 5.0f);
		ImGui::DragFloat("Bar Distance##HologramEffect", &mBarDistance, 0.01f, 0.0f, 5.0f);
		ImGui::DragFloat("Alpha##HologramEffect", &mAlpha, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Flicker Speed##HologramEffect", &mFlickerSpeed, 0.01f, 0.65f, 1.0f);
		ImGui::DragFloat("Rim Power##HologramEffect", &mRimPower, 0.01f, 0.0f, 5.0f);
		ImGui::DragFloat("Glow Distance##HologramEffect", &mGlowDistance, 0.01f, 0.0f, 5.0f);
		ImGui::DragFloat("Glow Speed##HologramEffect", &mGlowSpeed, 0.01f, 0.0f, 5.0f);
		ImGui::ColorEdit3("Rim Color##HologramEffect", &mRimColor.r);
		ImGui::ColorEdit3("Main Color##HologramEffect", &mMainColor.r);
		ImGui::DragFloat("Glitch Intensity##HologramEffect", &mGlitchIntensity, 0.01f, 0.0f, 5.0f);
		ImGui::DragFloat("Glitch Speed##HologramEffect", &mGlitchSpeed, 0.01f, 0.0f, 5.0f);
	}
}