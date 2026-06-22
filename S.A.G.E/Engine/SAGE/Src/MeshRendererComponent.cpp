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
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(MeshRendererComponent, 500);

void MeshRendererComponent::LoadComponentFromTemplate(const rapidjson::Value& value)
{
	// TODO: 
}

void MeshRendererComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	// TODO: 
}

void MeshRendererComponent::Initialize()
{
	mRenderService = GetOwner().GetWorld().GetService<RenderService>();
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();

	auto tm = TextureManager::Get();
	RenderObject& renderObject = mMeshFilter->GetRenderObject();
	renderObject.diffuseMapId = tm->LoadTexture("../Sprites/HalfLife/Surfaces/c1a0_w1d1.png"); // TODO: 
	//renderObject.specularMapId = tm->LoadTexture(fileName);
	//renderObject.bumpMapId = tm->LoadTexture(fileName);
	//renderObject.normalMapId = tm->LoadTexture(fileName);
}

void MeshRendererComponent::Terminate()
{
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

	// TODO: Move to a delegate call from scale changing.
	if (mTransformComponent != nullptr)
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

			if (mTileToZScale && mTilingSize.y != scale.y)
			{
				SetTilingSize(mTilingSize.x, scale.z);
			}
		}
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
}

void MeshRendererComponent::SetTileToYScale(bool tileToYScale)
{
	if (mTileToYScale == tileToYScale)
	{
		return;
	}

	mTileToYScale = tileToYScale;
}

void MeshRendererComponent::SetTileToZScale(bool tileToZScale)
{
	if (mTileToZScale == tileToZScale)
	{
		return;
	}

	mTileToZScale = tileToZScale;
}