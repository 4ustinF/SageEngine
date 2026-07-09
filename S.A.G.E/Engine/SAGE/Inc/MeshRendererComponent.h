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
		bool GetShouldBloom() const { return mAllowBloom; }
		bool GetIsTransparent() const { return mIsTransparent; }

		// Setters
		void SetMeshFilterComponent(MeshFilterComponent* meshFilterComponent) { mMeshFilter = meshFilterComponent; }
		void SetDiffuseMapFileName(const char* fileName) { mMaterialData.diffuseMapName = fileName; }					// TODO: Might need to update mMaterialData when these update as well.
		void SetSpecularMapFileName(const char* fileName) { mMaterialData.specularMapName = fileName; }					// TODO: Might need to update mMaterialData when these update as well.
		void SetBumpMapFileName(const char* fileName) { mMaterialData.bumpMapName = fileName; }							// TODO: Might need to update mMaterialData when these update as well.
		void SetNormalMapFileName(const char* fileName) { mMaterialData.normalMapName = fileName; }						// TODO: Might need to update mMaterialData when these update as well.
		void SetTilingSize(float xTilingSize, float yTilingSize);
		void SetTilingSize(const SAGE::Math::Vector2& tilingSize);
		void SetTilingOffset(float xTilingOffset, float yTilingOffset);
		void SetTilingOffset(const SAGE::Math::Vector2& tilingOffset);
		void SetTileToScale(bool tileToXScale = false, bool tileToYScale = false, bool tileToZScale = false);
		void SetTileToXScale(bool tileToXScale);
		void SetTileToYScale(bool tileToYScale);
		void SetTileToZScale(bool tileToYScale);

	private:
		MeshFilterComponent* mMeshFilter = nullptr;
		RenderService* mRenderService = nullptr;
		TransformComponent* mTransformComponent = nullptr;
		SAGE::Graphics::TextureManager* mTextureManager = nullptr;

		void TextureDebugUI(const char* mapName, SAGE::Graphics::TextureId& textureId, std::string& filePath);
		void MaterialDebugUI();
		void MaterialDataDebugUI(const char* label, SAGE::Graphics::Color& renderObjectColor, SAGE::Graphics::Color& materialDataColor);
		const float mMaxPreviewSize = 64.0f;
		const std::string mMissingDiffuseMapFileName = "missing.png";
		SAGE::Graphics::TextureId mMissingTextureID;

		void UpdateScaleSizeDelegateHandle();
		void OnScaleSizeChanged(const SAGE::Math::Vector3& scale);
		SAGE::Core::Delegate::FDelegateHandle ScaleChangedHandle;

		SAGE::Math::Vector2 mTilingSize = SAGE::Math::Vector2::One;
		SAGE::Math::Vector2 mTilingOffset = SAGE::Math::Vector2::Zero;
		bool mTileToXScale = false;
		bool mTileToYScale = false;
		bool mTileToZScale = false;

		void LoadMaterial(std::filesystem::path filePath); // TODO: Make the material an object so if we change it in one place it updates everywhere else.
		void LoadTextures();
		SAGE::Graphics::Model::MaterialData mMaterialData; // TODO tidy up in here.
		std::string mMaterialFilePath = "";
		bool mIsBasic = false;
		bool mAllowBloom = false;
		bool mIsTransparent = false;
	};
}