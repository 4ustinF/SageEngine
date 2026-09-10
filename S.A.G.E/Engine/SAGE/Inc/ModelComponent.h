#pragma once

#include "Component.h"

namespace SAGE
{
	class ModelComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::Model)
		MEMORY_POOL_DECLARE

		const char* GetCompName() override { return "Model Component"; }
		void LoadComponentFromTemplate(const rapidjson::Value& value) override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		void DebugUI() override;
		void OnEnable() override;
		void OnDisable() override;

		Graphics::Model& GetModel();
		const Graphics::Model& GetModel() const;

		const Math::Vector3& GetRotation() const { return mRotation; }

		void SetFileName(const char* fileName) { mFileName = fileName; }
		void SetRotation(const Math::Vector3& rotation) { mRotation = rotation; }
		void SetCanCastShadows(bool canCast);
		
		void SetIsBasicModel(bool isBasic) { mIsBasicModel = isBasic; }

	private:
		void UpdateRenderGroupShadowSettings();

		std::string mFileName;
		Graphics::ModelId mModelId;
		Graphics::RenderGroup* mRenderGroup = nullptr;
		Math::Vector3 mRotation = Math::Vector3::Zero;

		bool mCanCastShadows = true;
		bool mIsBasicModel = false;
		float mIdentSize = 5.0f;
	};
}