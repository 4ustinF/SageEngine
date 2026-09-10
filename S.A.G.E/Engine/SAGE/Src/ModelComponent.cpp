#include "Precompiled.h"
#include "ModelComponent.h"

#include "GameObject.h"
#include "GameWorld.h"
#include "RenderService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(ModelComponent, 500);

void ModelComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	if (value.HasMember("FileName"))
	{
		const char* fileName = value["FileName"].GetString();
		SetFileName(fileName);
	}

	if (value.HasMember("Rotation"))
	{
		const auto& rotation = value["Rotation"].GetArray();
		const float x = rotation[0].GetFloat() * Math::Constants::DegToRad;
		const float y = rotation[1].GetFloat() * Math::Constants::DegToRad;
		const float z = rotation[2].GetFloat() * Math::Constants::DegToRad;
		SetRotation({ x, y, z });
	}

	if (value.HasMember("IsBasicModel"))
	{
		const bool isBasic = value["IsBasicModel"].GetBool();
		SetIsBasicModel(isBasic);
	}

	if (value.HasMember("Can Cast Shadows"))
	{
		const bool canCastShadows = value["Can Cast Shadows"].GetBool();
		SetCanCastShadows(canCastShadows);
	}
}

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

	// --- Can Cast Shadows ---
	if (!mCanCastShadows)
	{
		SaveBoolToTemplate(compObj, allocator, "Can Cast Shadows", mCanCastShadows);
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
		if (ImGui::Checkbox("Can Cast Shadows##ModelComponent", &mCanCastShadows))
		{
			UpdateRenderGroupShadowSettings();
		}

		Model& model = GetModel(); // TODO:
		if (mRenderGroup != nullptr)
		{
			int materialIndex = 0;
			for (RenderObject& renderObject : *mRenderGroup)
			{
				ImGui::PushID(materialIndex);
				const std::string headerText = "Render Object " + std::to_string(materialIndex++) + "##ModelComponent";

				if (ImGui::TreeNode(headerText.c_str()))
				{
					renderObject.DebugUI();
					ImGui::TreePop();
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
	UpdateRenderGroupShadowSettings();
}

void ModelComponent::OnDisable()
{
	auto renderService = GetOwner().GetWorld().GetService<RenderService>();
	renderService->Unregister(this, mIsBasicModel);
	mRenderGroup = nullptr;
}

Model& ModelComponent::GetModel()
{
	return const_cast<Model&>(*ModelManager::Get()->GetModel(mModelId));
}

const Model& ModelComponent::GetModel() const
{
	return *ModelManager::Get()->GetModel(mModelId);
}

void ModelComponent::SetCanCastShadows(bool canCast)
{
	mCanCastShadows = canCast;
	UpdateRenderGroupShadowSettings();
}

void ModelComponent::UpdateRenderGroupShadowSettings()
{
	if (mRenderGroup == nullptr)
	{
		return;
	}

	for (RenderObject& renderObject : *mRenderGroup)
	{
		renderObject.canCastShadows = mCanCastShadows;
	}
}