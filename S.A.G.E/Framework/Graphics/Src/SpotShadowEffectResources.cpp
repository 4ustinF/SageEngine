#include "Precompiled.h"
#include "SpotShadowEffectResources.h"

#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void SpotShadowEffectResources::Initialize()
{
	vertexShader.Initialize<Vertex>(L"../../Assets/Shaders/Shadow.fx");
	pixelShader.Initialize(L"../../Assets/Shaders/Shadow.fx");

	transformBuffer.Initialize();
	boneTransformBuffer.Initialize();
	settingsBuffer.Initialize();
}

void SpotShadowEffectResources::Terminate()
{
	settingsBuffer.Terminate();
	boneTransformBuffer.Terminate();
	transformBuffer.Terminate();

	pixelShader.Terminate();
	vertexShader.Terminate();
}
