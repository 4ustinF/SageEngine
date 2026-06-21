#pragma once

#include "Model.h"
#include "ModelIO.h"

namespace SAGE::Graphics
{
	using ModelId = std::size_t;
	class ModelManager final
	{
	public:
		static void StaticInitialize();
		static void StaticTerminate();
		static ModelManager* Get();

	public:
		ModelId LoadModel(std::filesystem::path fileName)
		{
			auto modelId = std::filesystem::hash_value(fileName);
			auto [iter, success] = mInventory.insert({modelId, nullptr});
			if (success)
			{
				auto& modelPtr = iter->second;
				modelPtr = std::make_unique<Model>();
				ModelIO::LoadModel(fileName, *modelPtr);
				ModelIO::LoadMaterial(fileName, *modelPtr);
				ModelIO::LoadSkeleton(fileName, *modelPtr);
				ModelIO::LoadAnimationSet(fileName, *modelPtr);
			}
			return modelId;
		}

		Model* GetModel(ModelId modelId)
		{
			auto iter = mInventory.find(modelId);
			return iter != mInventory.end() ? iter->second.get() : nullptr;
		}

		const Model* GetModel(ModelId modelId) const
		{
			auto iter = mInventory.find(modelId);
			return iter != mInventory.end() ? iter->second.get() : nullptr;
		}

		const Model* GetModel(std::filesystem::path fileName) const
		{
			auto modelId = std::filesystem::hash_value(fileName);
			return GetModel(modelId);
		}

	private:
		std::unordered_map<ModelId, std::unique_ptr<Model>> mInventory;
	};
}