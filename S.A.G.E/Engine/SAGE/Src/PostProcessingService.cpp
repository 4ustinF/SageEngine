#include "Precompiled.h"
#include "PostProcessingService.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void PostProcessingService::Initialize()
{
	SetServiceName("Post Processing Service");

	mPostProccessingEffect.Initialize();
	mPostProccessingEffect.SetTexture(&mRenderTarget);
	mPostProccessingEffect.SetMode(PostProcessingEffect::Mode::None);
	mPostProccessingEffect.SetIntensity(25.0f);

	auto gs = GraphicsSystem::Get();
	const auto screenWidth = gs->GetBackBufferWidth();
	const auto screenHeight = gs->GetBackBufferHeight();
	mRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);

	mScreenQuad.meshBuffer.Initialize(MeshBuilder::CreateScreenQuad());
}

void PostProcessingService::Terminate()
{
	mScreenQuad.Terminate();
	mRenderTarget.Terminate();
	mPostProccessingEffect.Terminate();
}

void PostProcessingService::NewRender()
{
	mPostProccessingEffect.Begin();
	mPostProccessingEffect.Render(mScreenQuad);
	mPostProccessingEffect.End();
}

void PostProcessingService::DebugUI()
{
	mPostProccessingEffect.DebugUI();

	//ImGui::Begin("Post Processing Service", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	//
	//ImGui::Separator();
	//
	//ImGui::End();
}
