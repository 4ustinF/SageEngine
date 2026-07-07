#pragma once

#include "Component.h"

namespace SAGE
{
	class TransformComponent;

	enum class MeshType // TODO: If you modify this list make sure to also modify MeshTypeNames.
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

		const char* GetCompName() override { return "Mesh Filter Component"; }
		void LoadComponentFromTemplate(const rapidjson::Value& value) override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		void Initialize() override;
		void Terminate() override;

		void DebugUI() override;

		// Getters
		SAGE::Graphics::RenderObject& GetRenderObject() { return mRenderObject; };

		// Setters
		void SetMeshType(MeshType meshType) { mMeshType = meshType; }
		void SetPivotType(SAGE::Input::Pivot pivot) { mPivot = pivot; }
		void SetDivisions(SAGE::Math::Vector2Int divisions) { mDivisions = divisions; }
		void SetSpacing(SAGE::Math::Vector2 spacing) { mSpacing = spacing; }
		void SetFlipVertices(bool flipVertices) { mFlipVertices = flipVertices; }
		void SetRadius(float radius) { mRadius = radius; }

		// Helpers
		MeshType StringToMeshType(const std::string& meshType);

	private:
		void GenerateMesh();
		void GenerateCubeMesh();
		void GenerateCylinderMesh();
		void GeneratePlaneMesh();
		void GenerateQuadMesh();
		void GenerateSphereMesh();
		void GenerateCustomMesh();

		TransformComponent* mTransformComponent = nullptr;

		SAGE::Graphics::RenderObject mRenderObject;
		MeshType mMeshType = MeshType::Cube;
		SAGE::Graphics::Mesh mMesh;
		bool mEnableWireframe = true;

		std::string mCustomFilePath = ""; // TODO: Convert to std::filesystem::path
		SAGE::Input::Pivot mPivot = Input::Pivot::Center;
		Math::Vector2Int mDivisions = Math::Vector2Int::One;
		Math::Vector2 mSpacing = Math::Vector2::One;
		bool mFlipVertices = false;
		float mRadius = 1.0f;

		const char* MeshTypeNames[6] =
		{
			"Cube",
			"Cylinder",
			"Plane",
			"Quad",
			"Sphere",
			"Custom"
		};
	};
}