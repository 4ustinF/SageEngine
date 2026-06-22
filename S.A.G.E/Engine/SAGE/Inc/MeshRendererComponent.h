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
		void SetDiffuseMapFileName(const char* fileName) { diffuseMapFileName = fileName; }
		void SetSpecularMapFileName(const char* fileName) { specularMapFileName = fileName; }
		void SetBumpMapFileName(const char* fileName) { bumpMapFileName = fileName; }
		void SetNormalMapFileName(const char* fileName) { normalMapFileName = fileName; }
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

		std::string diffuseMapFileName = "";
		std::string specularMapFileName = "";
		std::string bumpMapFileName = "";
		std::string normalMapFileName = "";

		SAGE::Math::Vector2 mTilingSize = SAGE::Math::Vector2::One;
		bool mTileToXScale = false;
		bool mTileToYScale = false;
		bool mTileToZScale = false;
	};
}