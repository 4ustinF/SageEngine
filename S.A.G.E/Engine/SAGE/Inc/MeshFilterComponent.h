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
		const Graphics::RenderObject& GetRenderObject() const { return mRenderObject; };
		Graphics::RenderObject& GetRenderObject() { return mRenderObject; };
		const Graphics::Mesh& GetMesh() const { return mMesh; };
		const Math::OBB GetGlobalBoundingBox() const;

		// Setters // TODO: These should should trigger an update on the mesh for when we call them other then the init func.
		void SetMeshType(MeshType meshType) { mMeshType = meshType; }
		void SetPivotType(Input::Pivot pivot) { mMeshFilterData.pivot = pivot; }
		void SetDivisions(Math::Vector2Int divisions) { mMeshFilterData.divisions = divisions; }
		void SetSpacing(Math::Vector2 spacing) { mMeshFilterData.spacing = spacing; }
		void SetFlipVertices(bool flipVertices) { mMeshFilterData.flipVertices = flipVertices; }
		void SetRadius(float radius) { mMeshFilterData.radius = radius; }

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
		void GenerateBoundingBox(); // We need to update the bounding box when the transform comp updates.

		TransformComponent* mTransformComponent = nullptr;

		Graphics::RenderObject mRenderObject;
		MeshType mMeshType = MeshType::Cube;
		Graphics::Mesh mMesh;

		struct MeshFilterData
		{
			std::string customFilePath = ""; // TODO: Convert to std::filesystem::path
			Input::Pivot pivot = Input::Pivot::Center;
			Math::Vector2Int divisions = Math::Vector2Int::One;
			Math::Vector2 spacing = Math::Vector2::One;
			bool flipVertices = false;
			float radius = 1.0f;
		};

		MeshFilterData mMeshFilterData;
		MeshFilterData mAdjustedMeshFilterData;

		const char* MeshTypeNames[6] =
		{
			"Cube",
			"Cylinder",
			"Plane",
			"Quad",
			"Sphere",
			"Custom"
		};

		// Debug
		bool mEnableWireframe = true;
		bool mFillWireframe = false;
		Math::OBB mLocalBoundingBox;
		int mDebugVertIndex = -1;
	};
}