#pragma once

#include "Component.h"
#include <Windows.h> // TODO: Move out
#include <commdlg.h> // TODO: Move out

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
		void SetDiffuseMapFileName(const char* fileName) { mMaterialData.diffuseMapName = fileName; }					// TODO: Might need to update mMaterialData when these update as well.
		void SetSpecularMapFileName(const char* fileName) { mMaterialData.specularMapName = fileName; }					// TODO: Might need to update mMaterialData when these update as well.
		void SetBumpMapFileName(const char* fileName) { mMaterialData.bumpMapName = fileName; }							// TODO: Might need to update mMaterialData when these update as well.
		void SetNormalMapFileName(const char* fileName) { mMaterialData.normalMapName = fileName; }						// TODO: Might need to update mMaterialData when these update as well.
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
		bool mTileToXScale = false;
		bool mTileToYScale = false;
		bool mTileToZScale = false;

		void LoadMaterial(std::filesystem::path filePath);
		void LoadTextures();
		SAGE::Graphics::Model::MaterialData mMaterialData; // TODO tidy up in here.
		std::string mMaterialFilePath = "";
		bool mIsBasic = false; // TODO: Don't default true

		// TODO: Move out
		std::string OpenFileDialog(const char* fileFilterType)
		{
			char fileName[MAX_PATH] = "";

			OPENFILENAMEA ofn = {};
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = MAX_PATH;

			ofn.lpstrFilter = fileFilterType;

			ofn.Flags =
				OFN_FILEMUSTEXIST |
				OFN_PATHMUSTEXIST |
				OFN_NOCHANGEDIR;

			if (GetOpenFileNameA(&ofn))
			{
				return fileName;
			}

			return "";
		}
	};
}