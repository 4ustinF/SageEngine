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
	if (value.HasMember("DiffuseMapFileName"))
	{
		const auto& diffuseMapFileName = value["DiffuseMapFileName"].GetString();
		SetDiffuseMapFileName(diffuseMapFileName);
	}

	if (value.HasMember("SpecularMapFileName"))
	{
		const auto& specularMapFileName = value["SpecularMapFileName"].GetString();
		SetDiffuseMapFileName(specularMapFileName);
	}

	if (value.HasMember("BumpMapFileName"))
	{
		const auto& bumpMapFileName = value["BumpMapFileName"].GetString();
		SetBumpMapFileName(bumpMapFileName);
	}

	if (value.HasMember("NormalMapFileName"))
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
}

void MeshRendererComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	// --- Diffuse Map File Name ---
	if (!mDiffuseMapFileName.empty())
	{
		compObj.AddMember(
			rj::Value("DiffuseMapFileName", allocator),
			rj::Value(mDiffuseMapFileName.c_str(), allocator),
			allocator
		);
	}

	// --- Specular Map File Name ---
	if (!mSpecularMapFileName.empty())
	{
		compObj.AddMember(
			rj::Value("SpecularMapFileName", allocator),
			rj::Value(mSpecularMapFileName.c_str(), allocator),
			allocator
		);
	}

	// --- Bump Map File Name ---
	if (!mBumpMapFileName.empty())
	{
		compObj.AddMember(
			rj::Value("BumpMapFileName", allocator),
			rj::Value(mBumpMapFileName.c_str(), allocator),
			allocator
		);
	}

	// --- Normal Map File Name ---
	if (!mNormalMapFileName.empty())
	{
		compObj.AddMember(
			rj::Value("NormalMapFileName", allocator),
			rj::Value(mNormalMapFileName.c_str(), allocator),
			allocator
		);
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

	auto tm = TextureManager::Get();
	RenderObject& renderObject = mMeshFilter->GetRenderObject();

	if (!mDiffuseMapFileName.empty())
	{
		renderObject.diffuseMapId = tm->LoadTexture(mDiffuseMapFileName);
	}
	else
	{
		renderObject.diffuseMapId = tm->LoadTexture(mMissingDiffuseMapFileName);
	}

	if (!mSpecularMapFileName.empty())
	{
		renderObject.specularMapId = tm->LoadTexture(mSpecularMapFileName);
	}

	if (!mBumpMapFileName.empty())
	{
		renderObject.bumpMapId = tm->LoadTexture(mBumpMapFileName);
	}

	if (!mNormalMapFileName.empty())
	{
		renderObject.normalMapId = tm->LoadTexture(mNormalMapFileName);
	}
}

void MeshRendererComponent::Terminate()
{
	if (mTransformComponent != nullptr && ScaleChangedHandle.IsValid())
	{
		mTransformComponent->GetOnScaleChangeDelegate().Remove(ScaleChangedHandle);
	}

	mMeshFilter = nullptr;
	mTransformComponent = nullptr;
	mRenderService = nullptr;
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

		bool tileToXScale = mTileToXScale;
		if (ImGui::Checkbox("Tile To X Scale##MeshRendererComponent", &tileToXScale))
		{
			SetTileToXScale(tileToXScale);
		}

		bool tileToYScale = mTileToYScale;
		if (ImGui::Checkbox("Tile To Y Scale##MeshRendererComponent", &tileToYScale))
		{
			SetTileToYScale(tileToYScale);
		}

		bool tileToZScale = mTileToZScale;
		if (ImGui::Checkbox("Tile To Z Scale##MeshRendererComponent", &tileToZScale))
		{
			SetTileToZScale(tileToZScale);
		}

		//ImGui::ColorEdit4("Ambient##MeshRendererComponent", &materialData.material.ambient.r);
		//ImGui::ColorEdit4("Diffuse##MeshRendererComponent", &materialData.material.diffuse.r);
		//ImGui::ColorEdit4("Specular##MeshRendererComponent", &materialData.material.specular.r);
		//ImGui::ColorEdit4("Emissive##MeshRendererComponent", &materialData.material.emissive.r);
		//ImGui::DragFloat("Power##MeshRendererComponent", &materialData.material.power, 1.0f, 1.0f, 100.0f);
	}
}

void MeshRendererComponent::OnEnable()
{
	mRenderService->RegisterMeshRenderer(this, true); // TODO: Don't default true
}

void MeshRendererComponent::OnDisable()
{
	mRenderService->UnregisterMeshRenderer(this, true); // TODO: Don't default true
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

