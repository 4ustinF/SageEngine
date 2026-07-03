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
		mMaterialFilePath = value["MaterialFilePath"].GetString(); // TODO: Save value.
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

	mTextureManager = TextureManager::Get();
	RenderObject& renderObject = mMeshFilter->GetRenderObject();
	mMissingTextureID = mTextureManager->LoadTexture(mMissingDiffuseMapFileName);

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

	// TODO: Remove this is for debugging reasons
	renderObject.material.ambient = Color(0.2f, 0.2f, 0.2f, 1.0f);
	renderObject.material.diffuse = Color(0.2f, 0.2f, 0.2f, 1.0f);
	renderObject.material.specular = Color(0.2f, 0.2f, 0.2f, 1.0f);
	renderObject.material.emissive = Color(0.2f, 0.2f, 0.2f, 1.0f);
	renderObject.material.power = 25.0f;

	mMaterialData.material.ambient = renderObject.material.ambient;
	mMaterialData.material.diffuse = renderObject.material.diffuse;
	mMaterialData.material.specular = renderObject.material.specular;
	mMaterialData.material.emissive = renderObject.material.emissive;
	mMaterialData.material.power = renderObject.material.power;
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
		// TODO: Display map texture names?
		// TODO: Make it so we can swap out textures at run time.

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

		if (ImGui::ColorEdit4("Ambient##MeshRendererComponent", &renderObject.material.ambient.r))
		{
			mMaterialData.material.ambient = renderObject.material.ambient;
		}

		if (ImGui::ColorEdit4("Diffuse##MeshRendererComponent", &renderObject.material.diffuse.r))
		{
			mMaterialData.material.diffuse = renderObject.material.diffuse;
		}

		if (ImGui::ColorEdit4("Specular##MeshRendererComponent", &renderObject.material.specular.r))
		{
			mMaterialData.material.specular = renderObject.material.specular;
		}

		if (ImGui::ColorEdit4("Emissive##MeshRendererComponent", &renderObject.material.emissive.r))
		{
			mMaterialData.material.emissive = renderObject.material.emissive;
		}

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
		std::string newPath = OpenFileDialog();

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

	// might be good to uncomment this if the function goes public or goes anywhere thats not just in the load from template function?
	//RenderObject& renderObject = mMeshFilter->GetRenderObject();
	//renderObject.material = mMaterialData.material;
}