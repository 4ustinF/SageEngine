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
		void SetDiffuseMapFileName(const char* fileName) { mDiffuseMapFileName = fileName; }
		void SetSpecularMapFileName(const char* fileName) { mSpecularMapFileName = fileName; }
		void SetBumpMapFileName(const char* fileName) { mBumpMapFileName = fileName; }
		void SetNormalMapFileName(const char* fileName) { mNormalMapFileName = fileName; }
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

		const std::string mMissingDiffuseMapFileName = "missing.png";
		std::string mDiffuseMapFileName = "";
		std::string mSpecularMapFileName = "";
		std::string mBumpMapFileName = "";
		std::string mNormalMapFileName = "";

		void UpdateScaleSizeDelegateHandle();
		void OnScaleSizeChanged(const SAGE::Math::Vector3& scale);
		SAGE::Core::Delegate::FDelegateHandle ScaleChangedHandle;

		SAGE::Math::Vector2 mTilingSize = SAGE::Math::Vector2::One;
		bool mTileToXScale = false;
		bool mTileToYScale = false;
		bool mTileToZScale = false;

		// TODO: Take in a .material file
		// Add debug info on .mat and just regular mDiffuseMapFileName maybe use the .mat to get the relevant info?

		// TODO: Move out
		std::string OpenFileDialog()
		{
			char fileName[MAX_PATH] = "";

			OPENFILENAMEA ofn = {};
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = MAX_PATH;

			ofn.lpstrFilter =
				"Image Files\0*.png;*.jpg;*.jpeg;*.dds;*.tga\0";

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