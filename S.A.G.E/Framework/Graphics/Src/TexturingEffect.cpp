#include "Precompiled.h"
#include "TexturingEffect.h"

#include "RenderObject.h"
#include "VertexTypes.h"
#include "Camera.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void TexturingEffect::Initialize()
{
	mVertexShader.Initialize<OldVertex>(L"../../Assets/Shaders/Texturing.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/Texturing.fx");

	mTransformBuffer.Initialize();

	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void TexturingEffect::Terminate()
{
	mSampler.Terminate();

	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void TexturingEffect::Begin()
{
	ASSERT(mCamera != nullptr, "TexturingEffect - No camera set");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mSampler.BindVS(0);
	mSampler.BindPS(0);
}

void TexturingEffect::End()
{

}

void TexturingEffect::Render(const RenderObject& renderObject)
{
	const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mCamera->GetViewMatrix();
	const auto& proj = mCamera->GetProjectionMatrix();

	mTransformBuffer.Update(Transpose(matWorld * view * proj));

	auto tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);
	tm->BindPS(renderObject.specularMapId, 1);
	tm->BindVS(renderObject.bumpMapId, 2);
	tm->BindPS(renderObject.normalMapId, 3);

	renderObject.meshBuffer.Render();
}

void TexturingEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void TexturingEffect::DebugUI()
{

}