#include "Precompiled.h"
#include "RenderObject.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void RenderObject::Terminate()
{
	diffuseMapId = 0;
	specularMapId = 0;
	bumpMapId = 0;
	normalMapId = 0;

	meshBuffer.Terminate();
}

RenderGroup SAGE::Graphics::CreateRenderGroup(const Model& model, const IAnimator* animator)
{
	RenderGroup renderGroup;
	renderGroup.reserve(model.meshData.size());
	auto tm = TextureManager::Get();

	auto TryLoadTexture = [tm](auto& texture, const auto& textureName)
	{
		if (!textureName.empty()) {
			texture = tm->LoadTexture(textureName);
		}
	};

	for (auto& meshData : model.meshData)
	{
		auto& renderObject = renderGroup.emplace_back();

		auto& materialData = model.materialData[meshData.materialIndex];
		renderObject.material = materialData.material;

		TryLoadTexture(renderObject.diffuseMapId, materialData.diffuseMapName);
		TryLoadTexture(renderObject.specularMapId, materialData.specularMapName);
		TryLoadTexture(renderObject.bumpMapId, materialData.bumpMapName);
		TryLoadTexture(renderObject.normalMapId, materialData.normalMapName);

		renderObject.skeleton = model.skeleton.get();
		renderObject.animator = animator;

		renderObject.meshBuffer.Initialize(meshData.mesh);
	}
	return renderGroup;
}

void SAGE::Graphics::CleanUpRenderGroup(RenderGroup& renderGroup)
{
	for (auto& renderObject : renderGroup)
	{
		renderObject.Terminate();
	}
}