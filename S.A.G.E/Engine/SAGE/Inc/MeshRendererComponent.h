#pragma once

#include "Component.h"

namespace SAGE
{
	class RenderService;
	class MeshFilterComponent;
	class TransformComponent;

	class MeshRendererComponent final 
		: public Component
	{
	public:
		SET_TYPE_ID(ComponentId::MeshRenderer)
		MEMORY_POOL_DECLARE

		virtual const char* GetCompName() { return "Mesh Renderer Component"; }

		void Initialize() override;
		void Terminate() override;

		void Update(float deltaTime) override;
		void DebugUI() override;

		void OnEnable() override;
		void OnDisable() override;

		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		// Getters
		SAGE::Graphics::RenderObject& GetRenderObject();

		// Setters
		void SetMeshFilterComponent(MeshFilterComponent* meshFilterComponent) { mMeshFilter = meshFilterComponent; }
		void SetTilingSize(const SAGE::Math::Vector2& tilingSize);

	private:
		MeshFilterComponent* mMeshFilter = nullptr;
		RenderService* mRenderService = nullptr;
		TransformComponent* mTransformComponent = nullptr;

		SAGE::Math::Vector2 mTilingSize = SAGE::Math::Vector2::One;
	};
}