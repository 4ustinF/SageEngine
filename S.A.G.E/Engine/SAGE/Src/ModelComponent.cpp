#include "Precompiled.h"
#include "ModelComponent.h"

#include "GameObject.h"
#include "GameWorld.h"
#include "RenderService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(ModelComponent, 1000);

void ModelComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Model Component##ModelComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Indent(mIdentSize);

		Vector2 prevTilingSize = mTilingSize;
		if (ImGui::DragFloat2("Tiling Size##ModelComponent", &mTilingSize.x, 0.5f, 0.5, 10))
		{
			// TODO:
			if (mTilingSize != prevTilingSize)
			{

			}
		}

		Model& model = GetModel();
		const int materialSize = static_cast<int>(model.materialData.size());
		for (int materialIndex = 0; materialIndex < materialSize; ++materialIndex)
		{
			const std::string headerText = "Material Data " + std::to_string(materialIndex) + "##ModelComponent";
			if (ImGui::CollapsingHeader(headerText.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
			{
				auto& materialData = model.materialData[materialIndex]; // TODO: Figure out why we cannot modify these values.
				ImGui::ColorEdit4("Ambient##ModelComponent", &materialData.material.ambient.r);
				ImGui::ColorEdit4("Diffuse##ModelComponent", &materialData.material.diffuse.r);
				ImGui::ColorEdit4("Specular##ModelComponent", &materialData.material.specular.r);
				ImGui::ColorEdit4("Emissive##ModelComponent", &materialData.material.emissive.r);
				ImGui::DragFloat("Power##ModelComponent", &materialData.material.power, 1.0f, 1.0f, 100.0f);
			}
		}
	}
}

void ModelComponent::OnEnable() 
{
	const char* modelFileName = mFileName.c_str();
	mModelId = ModelManager::Get()->LoadModel(modelFileName, mTilingSize);

	auto renderService = GetOwner().GetWorld().GetService<RenderService>();
	renderService->Register(this, mIsBasicModel);
}

void ModelComponent::OnDisable()
{
	auto renderService = GetOwner().GetWorld().GetService<RenderService>();
	renderService->Unregister(this, mIsBasicModel);
}

void ModelComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	// --- File Name ---
	compObj.AddMember(
		rj::Value("FileName", allocator),
		rj::Value(mFileName.c_str(), allocator),
		allocator
	);

	// --- Tiling Size ---
	{
		rj::Value position(rj::kArrayType);
		position.PushBack(this->mTilingSize.x, allocator);
		position.PushBack(this->mTilingSize.y, allocator);
		compObj.AddMember("TilingSize", position, allocator);
	}

	// --- Rotation (convert back to degrees!) ---
	{
		rj::Value rotation(rj::kArrayType);
		rotation.PushBack(mRotation.x * Constants::RadToDeg, allocator);
		rotation.PushBack(mRotation.y * Constants::RadToDeg, allocator);
		rotation.PushBack(mRotation.z * Constants::RadToDeg, allocator);
		compObj.AddMember("Rotation", rotation, allocator);
	}

	compObj.AddMember(
		rj::Value("IsBasicModel", allocator),
		rj::Value(mIsBasicModel),
		allocator
	);

	//{
	//	rj::Value isBasicModel(rj::type);
	//	compObj.AddMember("TilingSize", isBasicModel, allocator);
	//}
}


//if (value.HasMember("Rotation"))
//{
//	const auto& rotation = value["Rotation"].GetArray();
//	const float x = rotation[0].GetFloat() * Math::Constants::DegToRad;
//	const float y = rotation[1].GetFloat() * Math::Constants::DegToRad;
//	const float z = rotation[2].GetFloat() * Math::Constants::DegToRad;
//	modelComponent->SetRotation({ x, y, z });
//}
//if (value.HasMember("IsBasicModel"))
//{
//	const bool isBasic = value["IsBasicModel"].GetBool();
//	modelComponent->SetIsBasicModel(isBasic);
//}