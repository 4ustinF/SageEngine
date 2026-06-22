#include "Precompiled.h"
#include "MeshFilterComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(MeshFilterComponent, 500);

void MeshFilterComponent::LoadComponentFromTemplate(const rapidjson::Value& value)
{
	// TODO: 
}

void MeshFilterComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	// TODO: 
}

void MeshFilterComponent::Initialize()
{
	GenerateMesh();
	mMeshTypeName = MeshTypeToString(mMeshType);
}

void MeshFilterComponent::Terminate()
{
	mRenderObject.Terminate();
}

void MeshFilterComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Mesh Filter Component##MeshFilterComponent ", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		// TODO: Display mMeshType for now.
		// TODO: Make it so we can swap mesh data out.
		ImGui::Text("Mesh:"); 
		ImGui::SameLine(); 
		ImGui::Text(mMeshTypeName.c_str());
	}
}

std::string MeshFilterComponent::MeshTypeToString(MeshType meshType)
{
	switch (meshType)
	{
	case MeshType::Cube:
		return "Cube";
	case MeshType::Cylinder:
		return "Cylinder";
	case MeshType::Plane:
		return "Plane";
	case MeshType::Quad:
		return "Quad";
	case MeshType::Sphere:
		return "Sphere";
	case MeshType::Custom:
		return "Custom";
	}

	return "Missing";
}

void MeshFilterComponent::GenerateMesh()
{
	switch (mMeshType)
	{
	case MeshType::Cube:
		GenerateCubeMesh();
		break;
	case MeshType::Cylinder:
		GenerateCylinderMesh();
		break;
	case MeshType::Plane:
		GeneratePlaneMesh();
		break;
	case MeshType::Quad:
		GenerateQuadMesh();
		break;
	case MeshType::Sphere:
		GenerateSphereMesh();
		break;
	case MeshType::Custom:
		GenerateCustomMesh();
		break;
	}
}

void MeshFilterComponent::GenerateCubeMesh()
{
	mRenderObject.meshBuffer.Initialize(MeshBuilder::CreateCube());
}

void MeshFilterComponent::GenerateCylinderMesh()
{
	mRenderObject.meshBuffer.Initialize(MeshBuilder::CreateCylinder(1, 1)); // TODO: 
}

void MeshFilterComponent::GeneratePlaneMesh()
{
	mRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(1, 1, 1.0f)); // TODO: 
}

void MeshFilterComponent::GenerateQuadMesh()
{
	//mRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(1, 1, 1.0f)); // TODO:
	mRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(1, 1, Vector2(3.2512f, 4.064f))); // TODO:
}

void MeshFilterComponent::GenerateSphereMesh()
{
	mRenderObject.meshBuffer.Initialize(MeshBuilder::CreateSphere(1, 1, 1.0f)); // TODO: 
}

void MeshFilterComponent::GenerateCustomMesh()
{
	// TODO: Read mesh data from a file
}
