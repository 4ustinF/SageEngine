#include "Precompiled.h"
#include "MeshFilterComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(MeshFilterComponent, 1000);

void MeshFilterComponent::LoadComponentFromTemplate(const rapidjson::Value& value)
{
	if (value.HasMember("MeshType"))
	{
		const auto& meshType = value["MeshType"].GetString();
		SetMeshType(StringToMeshType(meshType));
	}

	if (value.HasMember("Custom Mesh File Path"))
	{
		const auto& filePath = value["Custom Mesh File Path"].GetString();
		mCustomFilePath = filePath;
	}

	if (value.HasMember("Pivot"))
	{
		const auto& pivot = value["Pivot"].GetString();
		SetPivotType(StringToPivot(pivot));
	}

	if (value.HasMember("Divisions"))
	{
		const auto& divisions = value["Divisions"].GetArray();
		const int x = divisions[0].GetInt();
		const int y = divisions[1].GetInt();
		SetDivisions(Vector2Int(x, y));
	}

	if (value.HasMember("Spacing"))
	{
		const auto& spacing = value["Spacing"].GetArray();
		const float x = spacing[0].GetFloat();
		const float y = spacing[1].GetFloat();
		SetSpacing(Vector2(x, y));
	}

	if (value.HasMember("FlipVertices"))
	{
		const auto& flipVertices = value["FlipVertices"].GetBool();
		SetFlipVertices(flipVertices);
	}

	if (value.HasMember("Radius"))
	{
		const auto& radius = value["Radius"].GetFloat();
		SetRadius(radius);
	}
}

void MeshFilterComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	SaveStringToTemplate(compObj, allocator, "MeshType", mMeshTypeName); // Mesh Type
	SaveStringToTemplate(compObj, allocator, "Custom Mesh File Path", mCustomFilePath); // Custom Mesh File Path
	if (mPivot != Pivot::Center) { SaveStringToTemplate(compObj, allocator, "Pivot", PivotToString(mPivot)); } // Pivot 
	SaveVector2IntToTemplate(compObj, allocator, "Divisions", mDivisions, Vector2Int::One); // Divisions
	SaveVector2ToTemplate(compObj, allocator, "Spacing", mSpacing, Vector2::One); // Spacing
	if (mFlipVertices == true) { SaveBoolToTemplate(compObj, allocator, "FlipVertices", mFlipVertices); } // Flip Vertices 
	SaveNumberToTemplate(compObj, allocator, "Radius", mRadius, 1.0f); // Radius
}

void MeshFilterComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();

	GenerateMesh();
	mMeshTypeName = MeshTypeToString(mMeshType);
}

void MeshFilterComponent::Terminate()
{
	mTransformComponent = nullptr;
	mRenderObject.Terminate();
}

void MeshFilterComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Mesh Filter Component##MeshFilterComponent ", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		// TODO: Make it so we can swap mesh data out.
		ImGui::Text("Mesh: %s", mMeshTypeName.c_str());

		// Display vertices count
		ImGui::Text("Vertex Count: %s", std::to_string(mRenderObject.meshBuffer.GetVertexCount()).c_str());

		// Display data accordingly per mesh type
		switch (mMeshType)
		{
		case MeshType::Cube:
			break;
		case MeshType::Cylinder:
			ImGui::Text("Divisions: %i %i", mDivisions.x, mDivisions.y);
			ImGui::Text("Radius: %f", mRadius);
			break;
		case MeshType::Plane:
		case MeshType::Quad:
			ImGui::Text("Divisions: %i %i", mDivisions.x, mDivisions.y);
			ImGui::Text("Spacing: %f %f", mSpacing.x, mSpacing.y);
			ImGui::Text("Radius: %f", mRadius);
			ImGui::Text("Flip Vertices: %s", mFlipVertices ? "true" : "false");
			ImGui::Text("Pivot: %s", PivotToString(mPivot).c_str());
			break;
		case MeshType::Sphere:
			ImGui::Text("Divisions: %i %i", mDivisions.x, mDivisions.y);
			ImGui::Text("Spacing: %f %f", mSpacing.x, mSpacing.y);
			ImGui::Text("Radius: %f", mRadius);
			break;
		case MeshType::Custom:
			break;
		}

		ImGui::Checkbox("Display Wireframe##MeshFilterComponent", &mEnableWireframe);
	}

	if (mEnableWireframe)
	{
		const int indexCount = static_cast<int>(mMesh.indices.size());
		if (indexCount > 3)
		{
			const Vector3& worldPos = mTransformComponent != nullptr ? mTransformComponent->GetPosition() : Vector3::Zero;
			for (int i = 0; i < indexCount; i += 3)
			{
				const Vector3 pos0 = mMesh.vertices[mMesh.indices[i]].position + worldPos;
				const Vector3 pos1 = mMesh.vertices[mMesh.indices[i + 1]].position + worldPos;
				const Vector3 pos2 = mMesh.vertices[mMesh.indices[i + 2]].position + worldPos;
				SimpleDraw::AddFace(pos0, pos1, pos2, Colors::Red); // TODO: Add a color option and a filled option.
			}
		}
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

MeshType MeshFilterComponent::StringToMeshType(const std::string& meshType)
{
	if (meshType == "Cube") { return MeshType::Cube; }
	if (meshType == "Cylinder") { return MeshType::Cylinder; }
	if (meshType == "Plane") { return MeshType::Plane; }
	if (meshType == "Quad") { return MeshType::Quad; }
	if (meshType == "Sphere") { return MeshType::Sphere; }
	if (meshType == "Custom") { return MeshType::Custom; }

	return MeshType::Custom;
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
	mMesh = MeshBuilder::CreateCube(); // TODO: Maybe take in some params like extend or spacing to make rectangular?
	mRenderObject.meshBuffer.Initialize(mMesh);
}

void MeshFilterComponent::GenerateCylinderMesh()
{
	mMesh = MeshBuilder::CreateCylinder(mDivisions.x, mDivisions.y); // And radius?
	mRenderObject.meshBuffer.Initialize(mMesh);
}

void MeshFilterComponent::GeneratePlaneMesh()
{
	mMesh = MeshBuilder::CreatePlane(mDivisions.x, mDivisions.y, mSpacing, mFlipVertices, mPivot);
	mRenderObject.meshBuffer.Initialize(mMesh);
}

void MeshFilterComponent::GenerateQuadMesh()
{
	mMesh = MeshBuilder::CreatePlane(mDivisions.x, mDivisions.y, mSpacing, mFlipVertices, mPivot);
	mRenderObject.meshBuffer.Initialize(mMesh);
}

void MeshFilterComponent::GenerateSphereMesh()
{
	mMesh = MeshBuilder::CreateSphere(mDivisions.x, mDivisions.y, mRadius);
	mRenderObject.meshBuffer.Initialize(mMesh);
}

void MeshFilterComponent::GenerateCustomMesh()
{
	if (mCustomFilePath.empty())
	{
		return;
	}

	FILE* file = nullptr;
	fopen_s(&file, mCustomFilePath.c_str(), "r"); // fopen_s(&file, filePath.u8string().c_str(), "r");
	if (file == nullptr)
		return;

	uint32_t vertexCount = 0;
	fscanf_s(file, "VertexCount: %u\n", &vertexCount);
	mMesh.vertices.resize(vertexCount);

	for (auto& vertex : mMesh.vertices)
	{
		fscanf_s(file, "%f %f %f %f %f %f %f %f %f %f %f %d %d %d %d %f %f %f %f\n",
			&vertex.position.x, &vertex.position.y, &vertex.position.z,
			&vertex.normal.x, &vertex.normal.y, &vertex.normal.z,
			&vertex.tangent.x, &vertex.tangent.y, &vertex.tangent.z,
			&vertex.uv.x, &vertex.uv.y,
			&vertex.boneIndices[0], &vertex.boneIndices[1], &vertex.boneIndices[2], &vertex.boneIndices[3],
			&vertex.boneWeights[0], &vertex.boneWeights[1], &vertex.boneWeights[2], &vertex.boneWeights[3]
		);
	}

	uint32_t indexCount = 0;
	fscanf_s(file, "IndexCount: %u\n", &indexCount);
	mMesh.indices.resize(indexCount);

	for (size_t i = 0; i < indexCount; i += 3)
	{
		fscanf_s(file, "%d %d %d\n",
			&mMesh.indices[i],
			&mMesh.indices[i + 1u],
			&mMesh.indices[i + 2u]);
	}

	fclose(file);
	mRenderObject.meshBuffer.Initialize(mMesh);
}
