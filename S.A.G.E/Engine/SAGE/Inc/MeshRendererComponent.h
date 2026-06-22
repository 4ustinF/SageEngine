#pragma once

#include "Component.h"

namespace SAGE
{
	class RenderService;
	class MeshFilterComponent;

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

	private:
		MeshFilterComponent* mMeshFilter = nullptr;
		RenderService* mRenderService = nullptr;
		TransformComponent* mTransformComponent = nullptr;
	};
}