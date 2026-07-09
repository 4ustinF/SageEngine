#include "Precompiled.h"
#include "MeshFilterComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(MeshFilterComponent, 300);

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
		mMeshFilterData.customFilePath = filePath;
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
	SaveStringToTemplate(compObj, allocator, "MeshType", MeshTypeNames[static_cast<int>(mMeshType)]); // Mesh Type
	SaveStringToTemplate(compObj, allocator, "Custom Mesh File Path", mMeshFilterData.customFilePath); // Custom Mesh File Path
	if (mMeshFilterData.pivot != Pivot::Center) { SaveStringToTemplate(compObj, allocator, "Pivot", PivotToString(mMeshFilterData.pivot)); } // Pivot 
	SaveVector2IntToTemplate(compObj, allocator, "Divisions", mMeshFilterData.divisions, Vector2Int::One); // Divisions
	SaveVector2ToTemplate(compObj, allocator, "Spacing", mMeshFilterData.spacing, Vector2::One); // Spacing
	if (mMeshFilterData.flipVertices == true) { SaveBoolToTemplate(compObj, allocator, "FlipVertices", mMeshFilterData.flipVertices); } // Flip Vertices 
	SaveNumberToTemplate(compObj, allocator, "Radius", mMeshFilterData.radius, 1.0f); // Radius
}

void MeshFilterComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	GenerateMesh();
	mAdjustedMeshFilterData = mMeshFilterData;
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
		int currentMeshItem = static_cast<int>(mMeshType);
		ImGui::Text("Mesh: "); ImGui::SameLine();
		if (ImGui::Combo(" ", &currentMeshItem, MeshTypeNames, IM_ARRAYSIZE(MeshTypeNames)))
		{
			MeshType currentMeshType = static_cast<MeshType>(currentMeshItem);
			if (mMeshType != currentMeshType)
			{
				mAdjustedMeshFilterData = MeshFilterData(); // Reset adjustedMeshFilterData.
				mMeshFilterData = mAdjustedMeshFilterData;
				mMeshType = currentMeshType;
				mRenderObject.meshBuffer.Terminate();
				GenerateMesh();
			}
		}

		// TODO: Make it so we can swap mesh data out.
		// TODO: Make it so we can modify mesh settings here.

		// Display vertices count
		ImGui::Text("Vertex Count: %s", std::to_string(mRenderObject.meshBuffer.GetVertexCount()).c_str());

		// Display data accordingly per mesh type
		switch (mMeshType)
		{
		case MeshType::Cube:
			break;
		case MeshType::Cylinder:
		case MeshType::Sphere:
			ImGui::Text("Divisions: %i %i", mMeshFilterData.divisions.x, mMeshFilterData.divisions.y);
			ImGui::Text("Radius: %f", mMeshFilterData.radius);
			break;
		case MeshType::Plane:
		case MeshType::Quad:
			ImGui::Text("Divisions: %i %i", mMeshFilterData.divisions.x, mMeshFilterData.divisions.y);
			ImGui::Text("Spacing: %f %f", mMeshFilterData.spacing.x, mMeshFilterData.spacing.y);
			ImGui::Text("Flip Vertices: %s", mMeshFilterData.flipVertices ? "true" : "false");
			ImGui::Text("Pivot: %s", PivotToString(mMeshFilterData.pivot).c_str());
			break;
		case MeshType::Custom:
			break;
		}

		ImGui::Checkbox("Display Wireframe##MeshFilterComponent", &mEnableWireframe);
		ImGui::Checkbox("Fill Wireframe##MeshFilterComponent", &mFillWireframe);

		if (ImGui::CollapsingHeader("Adjust Mesh##MeshFilterComponent ", ImGuiTreeNodeFlags_CollapsingHeader))
		{
			switch (mMeshType)
			{
			case MeshType::Cube:
				break;
			case MeshType::Cylinder:
			case MeshType::Sphere:
				ImGui::DragInt2("Divisions##MeshFilterComponent", &mAdjustedMeshFilterData.divisions.x, 0.1f);
				ImGui::DragFloat("Radius##MeshFilterComponent", &mAdjustedMeshFilterData.radius, 0.1f);
				break;
			case MeshType::Plane:
			case MeshType::Quad:
				ImGui::DragInt2("Divisions##MeshFilterComponent", &mAdjustedMeshFilterData.divisions.x, 0.1f);
				ImGui::DragFloat2("Spacing##MeshFilterComponent", &mAdjustedMeshFilterData.spacing.x, 0.1f);
				ImGui::Checkbox("Flip Vertices", &mAdjustedMeshFilterData.flipVertices);
				//ImGui::Text("Pivot: %s", PivotToString(mMeshFilterData.pivot).c_str()); // TODO: 
				break;
			case MeshType::Custom:
				if (ImGui::Button(mAdjustedMeshFilterData.customFilePath.empty() ? "None (Material)" : std::filesystem::path(mAdjustedMeshFilterData.customFilePath)
					.filename()
					.string()
					.c_str(),
					ImVec2(200.0f, 30.0f)))
				{
					std::string newPath = OpenFileDialog("Mesh Files\0*.mesh\0");

					if (!newPath.empty())
					{
						std::filesystem::path assetsRoot =
							std::filesystem::absolute("../../Assets");

						std::filesystem::path relativeToAssets =
							std::filesystem::relative(newPath, assetsRoot);

						std::filesystem::path finalPath =
							"../../Assets" / relativeToAssets;

						mAdjustedMeshFilterData.customFilePath = finalPath.generic_string();
					}
				}
				break;
			}

			if (ImGui::Button("Adjust Mesh"))
			{
				mMeshFilterData = mAdjustedMeshFilterData;
				GenerateMesh();
			}
		}
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

				mFillWireframe ? SimpleDraw::AddFilledFace(pos0, pos1, pos2, Colors::Red) : SimpleDraw::AddFace(pos0, pos1, pos2, Colors::Red); // TODO: Add a color option.
			}
		}

		SimpleDraw::AddOBB(mBoundingBox, Colors::Blue);
	}
}

MeshType MeshFilterComponent::StringToMeshType(const std::string& meshType)
{
	const int meshTypeNamesSize = IM_ARRAYSIZE(MeshTypeNames);
	for (int meshTypeNamesIndex = 0; meshTypeNamesIndex < meshTypeNamesSize; ++meshTypeNamesIndex)
	{
		if (meshType == MeshTypeNames[meshTypeNamesIndex])
		{
			return static_cast<MeshType>(meshTypeNamesIndex);
		}
	}

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

	GenerateBoundingBox();
}

void MeshFilterComponent::GenerateCubeMesh()
{
	mMesh = MeshBuilder::CreateCube(); // TODO: Maybe take in some params like extend or spacing to make rectangular?
	mRenderObject.meshBuffer.Initialize(mMesh);
}

void MeshFilterComponent::GenerateCylinderMesh()
{
	mMesh = MeshBuilder::CreateCylinder(mMeshFilterData.divisions.x, mMeshFilterData.divisions.y); // And radius?
	mRenderObject.meshBuffer.Initialize(mMesh);
}

void MeshFilterComponent::GeneratePlaneMesh()
{
	mMesh = MeshBuilder::CreatePlane(mMeshFilterData.divisions.x, mMeshFilterData.divisions.y, mMeshFilterData.spacing, mMeshFilterData.flipVertices, mMeshFilterData.pivot);
	mRenderObject.meshBuffer.Initialize(mMesh);
}

void MeshFilterComponent::GenerateQuadMesh()
{
	mMesh = MeshBuilder::CreatePlane(mMeshFilterData.divisions.x, mMeshFilterData.divisions.y, mMeshFilterData.spacing, mMeshFilterData.flipVertices, mMeshFilterData.pivot);
	mRenderObject.meshBuffer.Initialize(mMesh);
}

void MeshFilterComponent::GenerateSphereMesh()
{
	mMesh = MeshBuilder::CreateSphere(mMeshFilterData.divisions.x, mMeshFilterData.divisions.y, mMeshFilterData.radius);
	mRenderObject.meshBuffer.Initialize(mMesh);
}

void MeshFilterComponent::GenerateCustomMesh()
{
	if (mMeshFilterData.customFilePath.empty())
	{
		return;
	}

	FILE* file = nullptr;
	fopen_s(&file, mMeshFilterData.customFilePath.c_str(), "r"); // fopen_s(&file, filePath.u8string().c_str(), "r");
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

void MeshFilterComponent::GenerateBoundingBox() // TODO: Make it work if the object moves. Bind to transform comp and update aabb pos/rot/scale when the transform comp pos/rot/scale moves.
{
	mBoundingBox = OBB();

	const int vertCount = static_cast<int>(mMesh.vertices.size());
	if (vertCount == 0)
	{
		return;
	}

	Math::Vector3 minPos = mMesh.vertices[0].position;
	Math::Vector3 maxPos = minPos;

	for (size_t i = 1; i < mMesh.vertices.size(); ++i)
	{
		const auto& p = mMesh.vertices[i].position;

		minPos.x = std::min(minPos.x, p.x);
		minPos.y = std::min(minPos.y, p.y);
		minPos.z = std::min(minPos.z, p.z);

		maxPos.x = std::max(maxPos.x, p.x);
		maxPos.y = std::max(maxPos.y, p.y);
		maxPos.z = std::max(maxPos.z, p.z);
	}

	mBoundingBox.center = (minPos + maxPos) * 0.5f;
	mBoundingBox.extend = (maxPos - minPos) * 0.5f;
	mBoundingBox.rotation = mTransformComponent != nullptr ? mTransformComponent->GetRotation() : Math::Quaternion::Identity;
}