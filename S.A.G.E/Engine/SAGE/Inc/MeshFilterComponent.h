#pragma once

#include "Component.h"

namespace SAGE
{
	enum class MeshType
	{
		Cube,
		Cylinder,
		Plane,		// Many triangles.
		Quad,		// 2 triangles.
		Sphere,
		Custom
	};

	class MeshFilterComponent final 
		: public Component
	{
	public:
		SET_TYPE_ID(ComponentId::MeshFilter)
		MEMORY_POOL_DECLARE

		virtual const char* GetCompName() { return "Mesh Filter Component"; }

		void Initialize() override;
		void Terminate() override;

		void DebugUI() override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		// Getters
		SAGE::Graphics::RenderObject& GetRenderObject() { return mRenderObject; };

		// Setters
		void SetMeshType(MeshType meshType) { mMeshType = meshType; }

		// Helpers
		std::string MeshTypeToString(MeshType meshType); // TODO: Move to a utils class?

	private:
		void GenerateMesh();
		void GenerateCubeMesh();
		void GenerateCylinderMesh();
		void GeneratePlaneMesh();
		void GenerateQuadMesh();
		void GenerateSphereMesh();
		void GenerateCustomMesh();

		SAGE::Graphics::RenderObject mRenderObject;
		MeshType mMeshType = MeshType::Quad; // TODO: default on cube.
		std::string mMeshTypeName = "Cube";

		// TODO: Add support to take in and generate custom meshes.
	};
}