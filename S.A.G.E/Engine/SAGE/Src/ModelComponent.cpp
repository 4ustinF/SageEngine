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

void ModelComponent::Terminate()
{
	mRenderGroup = nullptr;
}

void ModelComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Model Component##ModelComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Indent(mIdentSize);

		Model& model = GetModel();
		if (mRenderGroup != nullptr)
		{
			int materialIndex = 0;
			for (RenderObject& renderObject : *mRenderGroup)
			{
				ImGui::PushID(materialIndex);
				const std::string headerText = "Material Data " + std::to_string(materialIndex++) + "##ModelComponent";
				if (ImGui::CollapsingHeader(headerText.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
				{
					ImGui::ColorEdit4("Ambient##ModelComponent", &renderObject.material.ambient.r);
					ImGui::ColorEdit4("Diffuse##ModelComponent", &renderObject.material.diffuse.r);
					ImGui::ColorEdit4("Specular##ModelComponent", &renderObject.material.specular.r);
					ImGui::ColorEdit4("Emissive##ModelComponent", &renderObject.material.emissive.r);
					ImGui::DragFloat("Power##ModelComponent", &renderObject.material.power, 1.0f, 1.0f, 100.0f);
				}
				ImGui::PopID();
			}
		}
	}
}

void ModelComponent::OnEnable() 
{
	const char* modelFileName = mFileName.c_str();
	mModelId = ModelManager::Get()->LoadModel(modelFileName);

	auto renderService = GetOwner().GetWorld().GetService<RenderService>();
	mRenderGroup = renderService->Register(this, mIsBasicModel);
}

void ModelComponent::OnDisable()
{
	auto renderService = GetOwner().GetWorld().GetService<RenderService>();
	renderService->Unregister(this, mIsBasicModel);
	mRenderGroup = nullptr;
}
