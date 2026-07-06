#pragma once

#include "Material.h"
#include "MeshBuffer.h"
#include "Model.h"
#include "Skeleton.h"
#include "TextureManager.h"
#include "Transform.h"

namespace SAGE::Graphics
{
	struct IAnimator;

	class RenderObject
	{
	public:
		void Terminate();

		Transform transform;

		Material material;

		TextureId diffuseMapId = 0;
		TextureId specularMapId = 0;
		TextureId bumpMapId = 0;
		TextureId normalMapId = 0;

		MeshBuffer meshBuffer;
		SAGE::Math::Vector2 tilingSize = SAGE::Math::Vector2::One;
		SAGE::Math::Vector2 tilingOffset = SAGE::Math::Vector2::One;

		const Skeleton* skeleton = nullptr;
		const IAnimator* animator = nullptr;
	};

	using RenderGroup = std::vector<RenderObject>;

	[[nodiscard]] RenderGroup CreateRenderGroup(const Model& model, const IAnimator* animator = nullptr);
	void CleanUpRenderGroup(RenderGroup& renderGroup);
}