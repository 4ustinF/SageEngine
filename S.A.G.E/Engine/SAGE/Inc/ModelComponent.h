#pragma once

#include "Component.h"

namespace SAGE
{
	class ModelComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::Model)
		MEMORY_POOL_DECLARE

		void DebugUI() override;
		void OnEnable() override;
		void OnDisable() override;

		Graphics::Model& GetModel() 
		{ 
			return const_cast<Graphics::Model&>(*Graphics::ModelManager::Get()->GetModel(mModelId));
		}
		const Graphics::Model& GetModel() const 
		{ 
			return *Graphics::ModelManager::Get()->GetModel(mModelId); 
		}

		const Math::Vector3& GetRotation() const { return mRotation; }

		void SetFileName(const char* fileName) { mFileName = fileName; }
		void SetRotation(const Math::Vector3& rotation) { mRotation = rotation; }

	private:
		std::string mFileName;
		Graphics::ModelId mModelId;
		Math::Vector3 mRotation = Math::Vector3::Zero;
	};
}