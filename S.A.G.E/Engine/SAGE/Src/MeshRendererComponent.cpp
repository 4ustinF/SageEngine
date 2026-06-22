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
		// TODO: mTilingSize
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

void MeshRendererComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	// TODO: 
}

RenderObject& MeshRendererComponent::GetRenderObject()
{
	return mMeshFilter->GetRenderObject();
}

void MeshRendererComponent::SetTilingSize(const Vector2& tilingSize)
{
	mTilingSize = tilingSize;
	mMeshFilter->GetRenderObject().tilingSize = mTilingSize;
}