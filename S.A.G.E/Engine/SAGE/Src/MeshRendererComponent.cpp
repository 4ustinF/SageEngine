#include "Precompiled.h"
#include "MeshRendererComponent.h"

#include "GameWorld.h"
#include "GameObject.h"
#include "RenderService.h"
#include "MeshFilterComponent.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::Core::Delegate;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(MeshRendererComponent, 500);

void MeshRendererComponent::LoadComponentFromTemplate(const rapidjson::Value& value)
{
	if (value.HasMember("MaterialFilePath"))
	{
		mMaterialFilePath = value["MaterialFilePath"].GetString();
		LoadMaterial(mMaterialFilePath);
	}

	if (mMaterialData.diffuseMapName.empty() && value.HasMember("DiffuseMapFileName"))
	{
		const auto& diffuseMapFileName = value["DiffuseMapFileName"].GetString();
		SetDiffuseMapFileName(diffuseMapFileName);
	}

	if (mMaterialData.specularMapName.empty() && value.HasMember("SpecularMapFileName"))
	{
		const auto& specularMapFileName = value["SpecularMapFileName"].GetString();
		SetSpecularMapFileName(specularMapFileName);
	}

	if (mMaterialData.bumpMapName.empty() && value.HasMember("BumpMapFileName"))
	{
		const auto& bumpMapFileName = value["BumpMapFileName"].GetString();
		SetBumpMapFileName(bumpMapFileName);
	}

	if (mMaterialData.normalMapName.empty() && value.HasMember("NormalMapFileName"))
	{
		const auto& normalMapFileName = value["NormalMapFileName"].GetString();
		SetDiffuseMapFileName(normalMapFileName);
	}

	if (mMaterialData.material.power == 0.0f && value.HasMember("Power"))
	{
		const auto& power = value["Power"].GetFloat();
		mMaterialData.material.power = power;
	}

	if (mMaterialData.material.ambient == Colors::Black && value.HasMember("Ambient"))
	{
		const auto& ambient = value["Ambient"].GetArray();
		const float x = ambient[0].GetFloat();
		const float y = ambient[1].GetFloat();
		const float z = ambient[2].GetFloat();
		const float w = ambient[3].GetFloat();
		mMaterialData.material.ambient = Color(x, y, z, w);
	}

	if (mMaterialData.material.diffuse == Colors::Black && value.HasMember("Diffuse"))
	{
		const auto& diffuse = value["Diffuse"].GetArray();
		const float x = diffuse[0].GetFloat();
		const float y = diffuse[1].GetFloat();
		const float z = diffuse[2].GetFloat();
		const float w = diffuse[3].GetFloat();
		mMaterialData.material.diffuse = Color(x, y, z, w);
	}

	if (mMaterialData.material.specular == Colors::Black && value.HasMember("Specular"))
	{
		const auto& specular = value["Specular"].GetArray();
		const float x = specular[0].GetFloat();
		const float y = specular[1].GetFloat();
		const float z = specular[2].GetFloat();
		const float w = specular[3].GetFloat();
		mMaterialData.material.specular = Color(x, y, z, w);
	}

	if (mMaterialData.material.emissive == Colors::Black && value.HasMember("Emissive"))
	{
		const auto& emissive = value["Emissive"].GetArray();
		const float x = emissive[0].GetFloat();
		const float y = emissive[1].GetFloat();
		const float z = emissive[2].GetFloat();
		const float w = emissive[3].GetFloat();
		mMaterialData.material.emissive = Color(x, y, z, w);
	}

	if (value.HasMember("TilingSize"))
	{
		const auto& tilingSize = value["TilingSize"].GetArray();
		const float x = tilingSize[0].GetFloat();
		const float y = tilingSize[1].GetFloat();
		SetTilingSize(Vector2(x, y));
	}

	if (value.HasMember("TileToXScale"))
	{
		const auto& tileToXScale = value["TileToXScale"].GetBool();
		SetTileToXScale(tileToXScale);
	}

	if (value.HasMember("TileToYScale"))
	{
		const auto& tileToYScale = value["TileToYScale"].GetBool();
		SetTileToYScale(tileToYScale);
	}

	if (value.HasMember("TileToZScale"))
	{
		const auto& tileToZScale = value["TileToZScale"].GetBool();
		SetTileToZScale(tileToZScale);
	}

	RenderObject& renderObject = mMeshFilter->GetRenderObject();
	renderObject.material = mMaterialData.material;
}

void MeshRendererComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	if (!mMaterialFilePath.empty())
	{
		Model model;
		model.materialData.push_back(mMaterialData);
		Model::MaterialData& materialData = model.materialData[0];
		materialData.diffuseMapName = static_cast<std::filesystem::path>(materialData.diffuseMapName).filename().string();
		materialData.specularMapName = static_cast<std::filesystem::path>(materialData.specularMapName).filename().string();
		materialData.bumpMapName = static_cast<std::filesystem::path>(materialData.bumpMapName).filename().string();
		materialData.normalMapName = static_cast<std::filesystem::path>(materialData.normalMapName).filename().string();

		ModelIO::SaveMaterial(mMaterialFilePath, model);
		compObj.AddMember(
			rj::Value("MaterialFilePath", allocator),
			rj::Value(mMaterialFilePath.c_str(), allocator),
			allocator
		);
	}
	else
	{
		// --- Diffuse Map File Name ---
		if (!mMaterialData.diffuseMapName.empty())
		{
			compObj.AddMember(
				rj::Value("DiffuseMapFileName", allocator),
				rj::Value(mMaterialData.diffuseMapName.c_str(), allocator),
				allocator
			);
		}

		// --- Specular Map File Name ---
		if (!mMaterialData.specularMapName.empty())
		{
			compObj.AddMember(
				rj::Value("SpecularMapFileName", allocator),
				rj::Value(mMaterialData.specularMapName.c_str(), allocator),
				allocator
			);
		}

		// --- Bump Map File Name ---
		if (!mMaterialData.bumpMapName.empty())
		{
			compObj.AddMember(
				rj::Value("BumpMapFileName", allocator),
				rj::Value(mMaterialData.bumpMapName.c_str(), allocator),
				allocator
			);
		}

		// --- Normal Map File Name ---
		if (!mMaterialData.normalMapName.empty())
		{
			compObj.AddMember(
				rj::Value("NormalMapFileName", allocator),
				rj::Value(mMaterialData.normalMapName.c_str(), allocator),
				allocator
			);
		}

		// --- Material Power ---
		if (mMaterialData.material.power != 0.0f)
		{
			rj::Value power(rj::kNumberType);
			power.SetFloat(mMaterialData.material.power);
			compObj.AddMember("Power", power, allocator);
		}

		// --- Material Ambient ---
		if (mMaterialData.material.ambient != Colors::Black)
		{
			rj::Value ambient(rj::kArrayType);
			ambient.PushBack(mMaterialData.material.ambient.x, allocator);
			ambient.PushBack(mMaterialData.material.ambient.y, allocator);
			ambient.PushBack(mMaterialData.material.ambient.z, allocator);
			ambient.PushBack(mMaterialData.material.ambient.w, allocator);
			compObj.AddMember("Ambient", ambient, allocator);
		}

		// --- Material Diffuse ---
		if (mMaterialData.material.diffuse != Colors::Black)
		{
			rj::Value diffuse(rj::kArrayType);
			diffuse.PushBack(mMaterialData.material.diffuse.x, allocator);
			diffuse.PushBack(mMaterialData.material.diffuse.y, allocator);
			diffuse.PushBack(mMaterialData.material.diffuse.z, allocator);
			diffuse.PushBack(mMaterialData.material.diffuse.w, allocator);
			compObj.AddMember("Diffuse", diffuse, allocator);
		}

		// --- Material Specular ---
		if (mMaterialData.material.specular != Colors::Black)
		{
			rj::Value specular(rj::kArrayType);
			specular.PushBack(mMaterialData.material.specular.x, allocator);
			specular.PushBack(mMaterialData.material.specular.y, allocator);
			specular.PushBack(mMaterialData.material.specular.z, allocator);
			specular.PushBack(mMaterialData.material.specular.w, allocator);
			compObj.AddMember("Specular", specular, allocator);
		}

		// --- Material Emissive ---
		if (mMaterialData.material.emissive != Colors::Black)
		{
			rj::Value emissive(rj::kArrayType);
			emissive.PushBack(mMaterialData.material.emissive.x, allocator);
			emissive.PushBack(mMaterialData.material.emissive.y, allocator);
			emissive.PushBack(mMaterialData.material.emissive.z, allocator);
			emissive.PushBack(mMaterialData.material.emissive.w, allocator);
			compObj.AddMember("Emissive", emissive, allocator);
		}
	}

	// --- Tiling Size ---
	if (mTilingSize != Vector2::One)
	{
		rj::Value tilingSize(rj::kArrayType);
		tilingSize.PushBack(mTilingSize.x, allocator);
		tilingSize.PushBack(mTilingSize.y, allocator);
		compObj.AddMember("TilingSize", tilingSize, allocator);
	}

	// --- Tile To X Scale ---
	if (mTileToXScale == true)
	{
		compObj.AddMember(
			rj::Value("TileToXScale", allocator),
			rj::Value(mTileToXScale),
			allocator
		);
	}

	// --- Tile To Y Scale ---
	if (mTileToYScale == true)
	{
		compObj.AddMember(
			rj::Value("TileToYScale", allocator),
			rj::Value(mTileToYScale),
			allocator
		);
	}

	// --- Tile To Z Scale ---
	if (mTileToZScale == true)
	{
		compObj.AddMember(
			rj::Value("TileToZScale", allocator),
			rj::Value(mTileToZScale),
			allocator
		);
	}
}

void MeshRendererComponent::Initialize()
{
	mRenderService = GetOwner().GetWorld().GetService<RenderService>();
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	UpdateScaleSizeDelegateHandle();
	LoadTextures();
}

void MeshRendererComponent::Terminate()
{
	if (mTransformComponent != nullptr && ScaleChangedHandle.IsValid())
	{
		mTransformComponent->GetOnScaleChangeDelegate().Remove(ScaleChangedHandle);
	}

	mMissingTextureID = 0;
	mMeshFilter = nullptr;
	mTransformComponent = nullptr;
	mRenderService = nullptr;
	mTextureManager = nullptr;
}

void MeshRendererComponent::Update(float deltaTime)
{
	// Update Render Objects Transform based off of our transform Comp.
	if (mTransformComponent != nullptr)
	{
		RenderObject& renderObject = mMeshFilter->GetRenderObject();
		renderObject.transform = mTransformComponent->GetTransform();
	}
}

void MeshRendererComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Mesh Renderer Component##MeshRendererComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (ImGui::DragFloat2("Tiling Size##MeshRendererComponent", &mTilingSize.x, 0.1f))
		{
			SetTilingSize(mTilingSize);
		}

		// Tile To Scale
		{
		ImGui::Text("Tile To Scale");
		ImGui::SameLine();
		bool tileToXScale = mTileToXScale;
		if (ImGui::Checkbox("X##TileX", &tileToXScale))
		{
			SetTileToXScale(tileToXScale);
		}

		ImGui::SameLine();
		bool tileToYScale = mTileToYScale;
		if (ImGui::Checkbox("Y##TileY", &tileToYScale))
		{
			SetTileToYScale(tileToYScale);
		}

		ImGui::SameLine();
		bool tileToZScale = mTileToZScale;
		if (ImGui::Checkbox("Z##TileZ", &tileToZScale))
		{
			SetTileToZScale(tileToZScale);
		}
		}

		RenderObject& renderObject = mMeshFilter->GetRenderObject();
		TextureDebugUI("Diffuse Map", renderObject.diffuseMapId, mMaterialData.diffuseMapName);
		TextureDebugUI("Specular Map", renderObject.specularMapId, mMaterialData.specularMapName);
		TextureDebugUI("Bump Map", renderObject.bumpMapId, mMaterialData.bumpMapName);
		TextureDebugUI("Normal Map", renderObject.normalMapId, mMaterialData.normalMapName);
		MaterialDebugUI();

		MaterialDataDebugUI("Ambient##MeshRendererComponent", renderObject.material.ambient, mMaterialData.material.ambient);
		MaterialDataDebugUI("Diffuse##MeshRendererComponent", renderObject.material.diffuse, mMaterialData.material.diffuse);
		MaterialDataDebugUI("Specular##MeshRendererComponent", renderObject.material.specular, mMaterialData.material.specular);
		MaterialDataDebugUI("Emissive##MeshRendererComponent", renderObject.material.emissive, mMaterialData.material.emissive);
		if (ImGui::DragFloat("Power##MeshRendererComponent", &renderObject.material.power, 1.0f, 1.0f, 100.0f))
		{
			mMaterialData.material.power = renderObject.material.power;
		}
	}
}

void MeshRendererComponent::OnEnable()
{
	mRenderService->RegisterMeshRenderer(this, mIsBasic);
}

void MeshRendererComponent::OnDisable()
{
	mRenderService->UnregisterMeshRenderer(this, mIsBasic);
}

RenderObject& MeshRendererComponent::GetRenderObject()
{
	return mMeshFilter->GetRenderObject();
}

void MeshRendererComponent::SetTilingSize(float xTilingSize, float yTilingSize)
{
	SetTilingSize(Vector2(xTilingSize, yTilingSize));
}

void MeshRendererComponent::SetTilingSize(const Vector2& tilingSize)
{
	mTilingSize = tilingSize;
	mMeshFilter->GetRenderObject().tilingSize = mTilingSize;
}

void MeshRendererComponent::SetTileToScale(bool tileToXScale, bool tileToYScale, bool tileToZScale)
{
	SetTileToXScale(tileToXScale);
	SetTileToYScale(tileToYScale);
	SetTileToZScale(tileToZScale);
}

void MeshRendererComponent::SetTileToXScale(bool tileToXScale)
{
	if (mTileToXScale == tileToXScale)
	{
		return;
	}

	mTileToXScale = tileToXScale;
	UpdateScaleSizeDelegateHandle();
}

void MeshRendererComponent::SetTileToYScale(bool tileToYScale)
{
	if (mTileToYScale == tileToYScale)
	{
		return;
	}

	mTileToYScale = tileToYScale;
	UpdateScaleSizeDelegateHandle();
}

void MeshRendererComponent::SetTileToZScale(bool tileToZScale)
{
	if (mTileToZScale == tileToZScale)
	{
		return;
	}

	mTileToZScale = tileToZScale;
	UpdateScaleSizeDelegateHandle();
}

void MeshRendererComponent::TextureDebugUI(const char* mapName, TextureId& textureId, std::string& filePath)
{
	ImGui::Text(mapName);
	Texture* texture = mTextureManager->GetTexture(textureId);
	if (texture == nullptr)
	{
		texture = mTextureManager->GetTexture(mMissingTextureID);
	}

	const float width = static_cast<float>(texture->GetWidth());
	const float height = static_cast<float>(texture->GetHeight());
	const float scale = mMaxPreviewSize / std::max(width, height);
	const ImVec2 previewSize(width * scale, height * scale);

	ImGui::BeginGroup();
	if (ImGui::ImageButton(texture->GetRawData(), previewSize))
	{
		std::string newPath = OpenFileDialog("Image Files\0*.png;*.jpg;*.jpeg;*.dds;*.tga\0");

		if (!newPath.empty())
		{
			const auto relativePath =
				std::filesystem::relative(
					newPath,
					mTextureManager->GetRootDirectory());

			filePath = relativePath.generic_string();
			textureId = mTextureManager->LoadTexture(filePath);
		}
	}

	ImGui::EndGroup();
	ImGui::SameLine();
	ImGui::BeginGroup();

	if (!filePath.empty())
	{
		const std::filesystem::path path(filePath);
		ImGui::Text("%s", path.filename().string().c_str());
	}

	if (textureId != 0)
	{
		if (ImGui::Button(("Clear##" + std::string(mapName)).c_str()))
		{
			filePath = "";
			textureId = 0;
		}
	}

	ImGui::EndGroup();
}

void MeshRendererComponent::MaterialDebugUI()
{
	if (ImGui::Button(mMaterialFilePath.empty() ? "None (Material)" : std::filesystem::path(mMaterialFilePath)
		.filename()
		.string()
		.c_str(),
		ImVec2(200.0f, 30.0f)))
	{
		std::string newPath = OpenFileDialog("Material Files\0*.material\0");

		if (!newPath.empty())
		{
			std::filesystem::path assetsRoot =
				std::filesystem::absolute("../../Assets");

			std::filesystem::path relativeToAssets =
				std::filesystem::relative(newPath, assetsRoot);

			std::filesystem::path finalPath =
				"../../Assets" / relativeToAssets;

			mMaterialFilePath = finalPath.generic_string();
			LoadMaterial(mMaterialFilePath);
		}
	}

	if (!mMaterialFilePath.empty())
	{
		ImGui::SameLine();
		if (ImGui::Button("Clear##Material"))
		{
			mMaterialFilePath = "";
		}
	}
}

void MeshRendererComponent::MaterialDataDebugUI(const char* label, Color& renderObjectColor, Color& materialDataColor)
{
	if (ImGui::ColorEdit4(label, &renderObjectColor.r))
	{
		materialDataColor = renderObjectColor;
	}
}

void MeshRendererComponent::UpdateScaleSizeDelegateHandle()
{
	if (mTransformComponent == nullptr)
	{
		return;
	}

	if (mTileToXScale || mTileToYScale || mTileToZScale)
	{
		if (!ScaleChangedHandle.IsValid())
		{
			ScaleChangedHandle = mTransformComponent->GetOnScaleChangeDelegate().AddRaw(this, &MeshRendererComponent::OnScaleSizeChanged);
			OnScaleSizeChanged(mTransformComponent->GetScale());
		}
	}
	else
	{
		if (ScaleChangedHandle.IsValid())
		{
			mTransformComponent->GetOnScaleChangeDelegate().Remove(ScaleChangedHandle);
		}
	}
}

void MeshRendererComponent::OnScaleSizeChanged(const Vector3& scale)
{
	if (mTileToXScale || mTileToYScale || mTileToZScale)
	{
		const Vector3& scale = mTransformComponent->GetScale();

		if (mTileToXScale && mTilingSize.x != scale.x)
		{
			SetTilingSize(scale.x, mTilingSize.y);
		}

		if (mTileToYScale && mTilingSize.y != scale.y)
		{
			SetTilingSize(mTilingSize.x, scale.y);
		}

		if (mTileToZScale && mTilingSize.y != scale.z)
		{
			SetTilingSize(mTilingSize.x, scale.z);
		}
	}
}

void MeshRendererComponent::LoadMaterial(std::filesystem::path filePath)
{
	filePath.replace_extension("material");
	FILE* file = nullptr;
	auto err = fopen_s(&file, filePath.u8string().c_str(), "r");
	if (err != 0 || file == nullptr) {
		printf("Error: failed to open file %s for loading.", filePath.u8string().c_str());
		return;
	}

	uint32_t numMaterials = 0;
	fscanf_s(file, "MaterialCount: %d\n", &numMaterials);

	auto TryReadTextureName = [&](auto& destName)
		{
			char textureName[1024]{};
			if (fscanf_s(file, "%s\n", textureName, (uint32_t)sizeof(textureName)) && strcmp(textureName, "none") != 0) {
				destName = filePath.replace_filename(textureName).u8string().c_str();
			}
		};

	auto& material = mMaterialData.material;
	fscanf_s(file, "%f %f %f %f\n", &material.ambient.r, &material.ambient.g, &material.ambient.b, &material.ambient.a);
	fscanf_s(file, "%f %f %f %f\n", &material.diffuse.r, &material.diffuse.g, &material.diffuse.b, &material.diffuse.a);
	fscanf_s(file, "%f %f %f %f\n", &material.specular.r, &material.specular.g, &material.specular.b, &material.specular.a);
	fscanf_s(file, "%f %f %f %f\n", &material.emissive.r, &material.emissive.g, &material.emissive.b, &material.emissive.a);
	fscanf_s(file, "%f\n", &material.power);

	TryReadTextureName(mMaterialData.diffuseMapName);
	TryReadTextureName(mMaterialData.specularMapName);
	TryReadTextureName(mMaterialData.bumpMapName);
	TryReadTextureName(mMaterialData.normalMapName);

	fclose(file);
	LoadTextures();
}

void MeshRendererComponent::LoadTextures()
{
	mTextureManager = TextureManager::Get();
	RenderObject& renderObject = mMeshFilter->GetRenderObject();
	mMissingTextureID = mMissingTextureID != 0 ? mMissingTextureID : mTextureManager->LoadTexture(mMissingDiffuseMapFileName);

	if (!mMaterialData.diffuseMapName.empty())
	{
		renderObject.diffuseMapId = mTextureManager->LoadTexture(mMaterialData.diffuseMapName);
	}
	else
	{
		renderObject.diffuseMapId = mMissingTextureID;
	}

	if (!mMaterialData.specularMapName.empty())
	{
		renderObject.specularMapId = mTextureManager->LoadTexture(mMaterialData.specularMapName);
	}

	if (!mMaterialData.bumpMapName.empty())
	{
		renderObject.bumpMapId = mTextureManager->LoadTexture(mMaterialData.bumpMapName);
	}

	if (!mMaterialData.normalMapName.empty())
	{
		renderObject.normalMapId = mTextureManager->LoadTexture(mMaterialData.normalMapName);
	}

	renderObject.material = mMaterialData.material;
}