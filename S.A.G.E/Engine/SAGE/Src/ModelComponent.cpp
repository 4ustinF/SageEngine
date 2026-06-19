#include "Precompiled.h"
#include "ModelComponent.h"

#include "GameObject.h"
#include "GameWorld.h"
#include "RenderService.h"

using namespace SAGE;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(ModelComponent, 1000);

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
	mModelId = ModelManager::Get()->LoadModel(modelFileName);

	auto renderService = GetOwner().GetWorld().GetService<RenderService>();
	renderService->Register(this, mIsBasicModel);
}

void ModelComponent::OnDisable()
{
	auto renderService = GetOwner().GetWorld().GetService<RenderService>();
	renderService->Unregister(this, mIsBasicModel);
}
