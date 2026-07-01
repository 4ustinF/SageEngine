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

void ModelComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	// --- File Name ---
	compObj.AddMember(
		rj::Value("FileName", allocator),
		rj::Value(mFileName.c_str(), allocator),
		allocator
	);

	// --- Rotation (convert back to degrees!) ---
	if (mRotation != Vector3::Zero)
	{
		rj::Value rotation(rj::kArrayType);
		rotation.PushBack(mRotation.x * Constants::RadToDeg, allocator);
		rotation.PushBack(mRotation.y * Constants::RadToDeg, allocator);
		rotation.PushBack(mRotation.z * Constants::RadToDeg, allocator);
		compObj.AddMember("Rotation", rotation, allocator);
	}

	// --- Is Basic Model ---
	if (mIsBasicModel == true)
	{
		compObj.AddMember(
			rj::Value("IsBasicModel", allocator),
			rj::Value(mIsBasicModel),
			allocator
		);
	}

}

void ModelComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Model Component##ModelComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Indent(mIdentSize);

		Model& model = GetModel();
		const int materialSize = static_cast<int>(model.materialData.size());
		for (int materialIndex = 0; materialIndex < materialSize; ++materialIndex)
		{
			const std::string headerText = "Material Data " + std::to_string(materialIndex) + "##ModelComponent";
			if (ImGui::CollapsingHeader(headerText.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
			{
				auto& materialData = model.materialData[materialIndex]; // TODO: Figure out why we cannot modify these values. Maybe needs to be a CreateRenderGroup?
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
	mModelId = ModelManager::Get()->LoadModel(modelFileName);

	auto renderService = GetOwner().GetWorld().GetService<RenderService>();
	renderService->Register(this, mIsBasicModel);
}

void ModelComponent::OnDisable()
{
	auto renderService = GetOwner().GetWorld().GetService<RenderService>();
	renderService->Unregister(this, mIsBasicModel);
}
