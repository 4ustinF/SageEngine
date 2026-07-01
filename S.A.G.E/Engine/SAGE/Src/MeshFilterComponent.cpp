#include "Precompiled.h"
#include "MeshFilterComponent.h"

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
	// --- Mesh Type ---
	if (!mMeshTypeName.empty())
	{
		compObj.AddMember(
			rj::Value("MeshType", allocator),
			rj::Value(mMeshTypeName.c_str(), allocator),
			allocator
		);
	}

	// --- Custom Mesh File Path ---
	if (!mCustomFilePath.empty())
	{
		compObj.AddMember(
			rj::Value("MeshType", allocator),
			rj::Value(mCustomFilePath.c_str(), allocator),
			allocator
		);
	}

	// --- Pivot ---
	if (mPivot != Pivot::Center)
	{
		compObj.AddMember(
			rj::Value("Pivot", allocator),
			rj::Value(PivotToString(mPivot).c_str(), allocator),
			allocator
		);
	}

	// --- Divisions ---
	if (mDivisions != Vector2Int::One)
	{
		rj::Value divisions(rj::kArrayType);
		divisions.PushBack(mDivisions.x, allocator);
		divisions.PushBack(mDivisions.y, allocator);
		compObj.AddMember("Divisions", divisions, allocator);
	}

	// --- Spacing ---
	if (mSpacing != Vector2::One)
	{
		rj::Value spacing(rj::kArrayType);
		spacing.PushBack(mSpacing.x, allocator);
		spacing.PushBack(mSpacing.y, allocator);
		compObj.AddMember("Spacing", spacing, allocator);
	}

	// --- Flip Vertices ---
	if (mFlipVertices == true)
	{
		compObj.AddMember("FlipVertices", mFlipVertices, allocator);
	}

	// --- Radius ---
	if (mRadius != 1.0f)
	{
		compObj.AddMember("Radius", mRadius, allocator);
	}
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
	mRenderObject.meshBuffer.Initialize(MeshBuilder::CreateCube());
}

void MeshFilterComponent::GenerateCylinderMesh()
{
	mRenderObject.meshBuffer.Initialize(MeshBuilder::CreateCylinder(mDivisions.x, mDivisions.y)); // And radius?
}

void MeshFilterComponent::GeneratePlaneMesh()
{
	mRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(mDivisions.x, mDivisions.y, mSpacing, mFlipVertices, mPivot));
}

void MeshFilterComponent::GenerateQuadMesh()
{
	mRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(mDivisions.x, mDivisions.y, mSpacing, mFlipVertices, mPivot));
}

void MeshFilterComponent::GenerateSphereMesh()
{
	mRenderObject.meshBuffer.Initialize(MeshBuilder::CreateSphere(mDivisions.x, mDivisions.y, mRadius));
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

	SAGE::Graphics::Model::MeshData meshData;
	SAGE::Graphics::Mesh& mesh = meshData.mesh;

	uint32_t vertexCount = 0;
	fscanf_s(file, "VertexCount: %u\n", &vertexCount);
	mesh.vertices.resize(vertexCount);

	for (auto& vertex : mesh.vertices)
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
	mesh.indices.resize(indexCount);

	for (size_t i = 0; i < indexCount; i += 3)
	{
		fscanf_s(file, "%d %d %d\n",
			&mesh.indices[i],
			&mesh.indices[i + 1u],
			&mesh.indices[i + 2u]);
	}

	fclose(file);

	mRenderObject.meshBuffer.Initialize(meshData.mesh);
}
