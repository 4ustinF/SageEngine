#include "Precompiled.h"
#include "PostProccessingEffect.h"

#include "GraphicsSystem.h"
#include "RenderObject.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void PostProcessingEffect::Initialize()
{
	mVertexShader.Initialize<VertexPX>(L"../../Assets/Shaders/PostProcess.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/PostProcess.fx");

	mPostProcessBuffer.Initialize();

	mSampler.Initialize(Sampler::Filter::Point, Sampler::AddressMode::Wrap);
}

void PostProcessingEffect::Terminate()
{
	mSampler.Terminate();

	mPostProcessBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void PostProcessingEffect::Begin()
{
	//ASSERT(mTexture != nullptr, "PostProcessingEffect -- no input texture!");

	mVertexShader.Bind();
	mPixelShader.Bind();

	PostProcessData data;
	data.mode = mMode;

	switch (mMode)
	{
	case Mode::None:
	case Mode::Monochrome:
	case Mode::Invert:
	case Mode::Mirror: {
		break;
	}
	case Mode::Grain: {
		//data.param0 = mDeltaTime;
		data.param0 = SAGE::Core::TimeUtil::GetTime();
		data.param1 = mIntensity;
		break;
	}
	case Mode::ObjectiveGrain: {
		//data.param0 = mDeltaTime;
		data.param0 = SAGE::Core::TimeUtil::GetTime();
		data.param1 = mIntensity;
		break;
	}
	case Mode::Vignette: {
		data.param0 = mIntensity;
		data.param1 = mVignetteExtend;
		break;
	}
	case Mode::Sinethresholder: {
		data.param0 = mIntensity;
		data.param1 = mSineAmount;
		break;
	}
	case Mode::Blur: {
		data.param0 = blurBrightness;
		data.param1 = static_cast<float>(blurAmount);
		data.param2 = blurOffset;
		break;
	}
	case Mode::Blur2: {
		auto gs = GraphicsSystem::Get();
		const float screenWidth = static_cast<float>(gs->GetBackBufferWidth());
		const float screenHeight = static_cast<float>(gs->GetBackBufferHeight());
		data.param0 = 4.0f / screenWidth;
		data.param1 = 4.0f / screenHeight;
		break;
	}
	case Mode::Combine2: {
		break;
	}
	case Mode::NightVision: {
		data.param0 = SAGE::Core::TimeUtil::GetTime();
		data.param1 = mGlowDistance;
		data.param2 = mGlowSpeed;
		break;
	}
	}
	mPostProcessBuffer.Update(data);
	mPostProcessBuffer.BindPS(0);

	mSampler.BindVS(0);
	for (int i = 0; i < mTextures.size(); ++i) {
		if (mTextures[i] != nullptr) {
			mTextures[i]->BindPS(i);
		}
	}
}

void PostProcessingEffect::End()
{
	for (int i = 0; i < mTextures.size(); ++i) {
		Texture::UnbindPS(i);
	}
}

void PostProcessingEffect::Render(const RenderObject& renderObject)
{
	renderObject.meshBuffer.Render();
}

void PostProcessingEffect::SetTexture(const Texture* texture, uint32_t slot)
{
	ASSERT(slot < mTextures.size(), "PostProcessing Effect --- Invalid slot index.");
	mTextures[slot] = texture;
}

void PostProcessingEffect::SetMode(Mode mode)
{
	mMode = mode;
}


void PostProcessingEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("PostProcessingEffect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int currentMode = static_cast<int>(mMode);
		const char* modeNames[] =
		{
			"None",
			"Monochrome",
			"Invert",
			"Mirror",
			"Grain",
			"Objective Grain",
			"Vignette",
			"Sinethresholder",
			"Blur",
			"Blur2",
			"Combine2",
			"Night Vision",
		};
		if (ImGui::Combo("Mode", &currentMode, modeNames, static_cast<int>(std::size(modeNames)))) {
			if (currentMode != static_cast<int>(mMode)) {
				mMode = static_cast<Mode>(currentMode);
			}
		}

		if (mMode == Mode::Grain || mMode == Mode::ObjectiveGrain) {
			if (ImGui::CollapsingHeader("Grain", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat("Intensity", &mIntensity, 0.01f);
			}
		}

		if (mMode == Mode::Vignette) {
			if (ImGui::CollapsingHeader("Vignette", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat("Intensity", &mIntensity, 0.01f);
				ImGui::DragFloat("Extend", &mVignetteExtend, 0.01f);
			}
		}

		if (mMode == Mode::Sinethresholder) {
			if (ImGui::CollapsingHeader("Sinethresholder", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat("Intensity", &mIntensity, 0.01f);
				ImGui::DragFloat("Extend", &mSineAmount, 0.01f);
			}
		}

		if (mMode == Mode::Blur) {
			if (ImGui::CollapsingHeader("Blur", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat("Brightness", &blurBrightness, 0.01f, 0.0f, 1.0f);
				ImGui::DragInt("Amount", &blurAmount, 0.01f, 0, 10);
				ImGui::DragFloat("OffSet", &blurOffset, 0.0001f, 0.0f, 1.0f);
			}
		}

		if (mMode == Mode::NightVision) {
			if (ImGui::CollapsingHeader("NightVision", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat("mGlowDistance##NightVision", &mGlowDistance, 0.01f, 0.0f, 5.0f);
				ImGui::DragFloat("mGlowSpeed##NightVision", &mGlowSpeed, 0.01f, 0.0f, 5.0f);
			}
		}
	}
}