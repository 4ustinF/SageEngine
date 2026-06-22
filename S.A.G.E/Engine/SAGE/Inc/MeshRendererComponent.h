#pragma once

#include "Component.h"

namespace SAGE
{
	class RenderService;
	class MeshFilterComponent;
	class TransformComponent;

	class MeshRendererComponent final 
		: public Component
	{
	public:
		SET_TYPE_ID(ComponentId::MeshRenderer)
		MEMORY_POOL_DECLARE

		const char* GetCompName() override { return "Mesh Renderer Component"; }
		void LoadComponentFromTemplate(const rapidjson::Value& value) override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		void Initialize() override;
		void Terminate() override;

		void Update(float deltaTime) override;
		void DebugUI() override;

		void OnEnable() override;
		void OnDisable() override;

		// Getters
		SAGE::Graphics::RenderObject& GetRenderObject();

		// Setters
		void SetMeshFilterComponent(MeshFilterComponent* meshFilterComponent) { mMeshFilter = meshFilterComponent; }
		void SetTilingSize(float xTilingSize, float yTilingSize);
		void SetTilingSize(const SAGE::Math::Vector2& tilingSize);
		void SetTileToScale(bool tileToXScale = false, bool tileToYScale = false, bool tileToZScale = false);
		void SetTileToXScale(bool tileToXScale);
		void SetTileToYScale(bool tileToYScale);
		void SetTileToZScale(bool tileToYScale);

	private:
		MeshFilterComponent* mMeshFilter = nullptr;
		RenderService* mRenderService = nullptr;
		TransformComponent* mTransformComponent = nullptr;

		SAGE::Math::Vector2 mTilingSize = SAGE::Math::Vector2::One;
		bool mTileToXScale = false;
		bool mTileToYScale = false;
		bool mTileToZScale = false;

		// TODO: Make it so tiling size matches scale.
	};
}