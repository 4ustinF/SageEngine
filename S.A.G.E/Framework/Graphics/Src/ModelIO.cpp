#include "Precompiled.h"
#include "ModelIO.h"

#include "Model.h"
#include "AnimationIO.h"

using namespace SAGE;
using namespace SAGE::Graphics;

namespace
{
	void WriteMatrix(FILE* file, const Math::Matrix4& m)
	{
		fprintf(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
			m._11, m._12, m._13, m._14,
			m._21, m._22, m._23, m._24,
			m._31, m._32, m._33, m._34,
			m._41, m._42, m._43, m._44);
	}

	void ReadMatrix(FILE* file, const Math::Matrix4& m)
	{
		fscanf_s(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
			&m._11, &m._12, &m._13, &m._14,
			&m._21, &m._22, &m._23, &m._24,
			&m._31, &m._32, &m._33, &m._34,
			&m._41, &m._42, &m._43, &m._44);
	}
}

void ModelIO::SaveModel(std::filesystem::path filePath, const Model& model)
{
	if (model.meshData.empty())
		return;

	FILE* file = nullptr;
	fopen_s(&file, filePath.u8string().c_str(), "w");
	if (file == nullptr)
		return;

	const uint32_t meshCount = static_cast<uint32_t>(model.meshData.size());
	fprintf_s(file, "MeshCount: %u\n", meshCount);
	for (uint32_t i = 0; i < meshCount; ++i)
	{
		const auto& meshData = model.meshData[i];
		fprintf_s(file, "MaterialIndex: %u\n", meshData.materialIndex);

		const auto& mesh = meshData.mesh;
		const uint32_t vertexCount = static_cast<uint32_t>(mesh.vertices.size());
		fprintf_s(file, "VertexCount: %u\n", vertexCount);

		for (auto& vertex : mesh.vertices)
		{
			fprintf_s(file, "%f %f %f %f %f %f %f %f %f %f %f %d %d %d %d %f %f %f %f\n",
				vertex.position.x, vertex.position.y, vertex.position.z,
				vertex.normal.x, vertex.normal.y, vertex.normal.z,
				vertex.tangent.x, vertex.tangent.y, vertex.tangent.z,
				vertex.uv.x, vertex.uv.y,
				vertex.boneIndices[0], vertex.boneIndices[1], vertex.boneIndices[2], vertex.boneIndices[3],
				vertex.boneWeights[0], vertex.boneWeights[1], vertex.boneWeights[2], vertex.boneWeights[3]
			);
		}

		const uint32_t indexCount = static_cast<uint32_t>(mesh.indices.size());
		fprintf_s(file, "IndexCount: %u\n", indexCount);

		for (size_t i = 0; i < indexCount; i += 3)
		{
			fprintf_s(file, "%d %d %d\n",
				mesh.indices[i],
				mesh.indices[i + 1u],
				mesh.indices[i + 2u]);
		}
	}

	fclose(file);
}

void ModelIO::LoadModel(std::filesystem::path filePath, Model& model)
{
	FILE* file = nullptr;
	fopen_s(&file, filePath.u8string().c_str(), "r");
	if (file == nullptr)
		return;

	uint32_t meshCount = 0;
	fscanf_s(file, "MeshCount: %u\n", &meshCount);

	model.meshData.resize(meshCount);
	for (uint32_t i = 0; i < meshCount; ++i)
	{
		auto& meshData = model.meshData[i];
		fscanf_s(file, "MaterialIndex: %u\n", &meshData.materialIndex);

		auto& mesh = meshData.mesh;

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
	}

	fclose(file);
}

void ModelIO::SaveMaterial(std::filesystem::path filePath, const Model& model)
{
	if (model.materialData.empty()) {
		return;
	}

	filePath.replace_extension("material");

	FILE* file = nullptr;
	auto err = fopen_s(&file, filePath.u8string().c_str(), "w");
	if (err != 0 || file == nullptr) {
		return;
	}

	fprintf_s(file, "MaterialCount: %d\n", static_cast<uint32_t>(model.materialData.size()));
	for (const auto& materialData : model.materialData)
	{
		const auto& material = materialData.material;
		fprintf_s(file, "%f %f %f %f\n", material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a);
		fprintf_s(file, "%f %f %f %f\n", material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a);
		fprintf_s(file, "%f %f %f %f\n", material.specular.r, material.specular.g, material.specular.b, material.specular.a);
		fprintf_s(file, "%f\n", material.power);

		fprintf_s(file, "%s\n", materialData.diffuseMapName.empty() ? "none" : materialData.diffuseMapName.c_str());
		fprintf_s(file, "%s\n", materialData.specularMapName.empty() ? "none" : materialData.specularMapName.c_str());
		fprintf_s(file, "%s\n", materialData.bumpMapName.empty() ? "none" : materialData.bumpMapName.c_str());
		fprintf_s(file, "%s\n", materialData.normalMapName.empty() ? "none" : materialData.normalMapName.c_str());
	}
	fclose(file);
}

void ModelIO::LoadMaterial(std::filesystem::path filePath, Model& model)
{
	filePath.replace_extension("material");
	FILE* file = nullptr;
	auto err = fopen_s(&file, filePath.u8string().c_str(), "r");
	if (err != 0 || file == nullptr) {
		printf("Error: failed to open file %s for loading.", filePath.u8string().c_str());
		return;
	}

	uint32_t numMaterials = 0;
	fscanf_s(file, "MaterialCount: %d\n", &numMaterials);

	auto TryReadTextureName = [&](auto& destName)
	{
		char textureName[1024]{};
		if (fscanf_s(file, "%s\n", textureName, (uint32_t)sizeof(textureName)) && strcmp(textureName, "none") != 0) {
			destName = filePath.replace_filename(textureName).u8string().c_str();
		}
	};

	model.materialData.resize(numMaterials);
	for (auto& materialData : model.materialData)
	{
		auto& material = materialData.material;

		fscanf_s(file, "%f %f %f %f\n", &material.ambient.r, &material.ambient.g, &material.ambient.b, &material.ambient.a);
		fscanf_s(file, "%f %f %f %f\n", &material.diffuse.r, &material.diffuse.g, &material.diffuse.b, &material.diffuse.a);
		fscanf_s(file, "%f %f %f %f\n", &material.specular.r, &material.specular.g, &material.specular.b, &material.specular.a);
		fscanf_s(file, "%f\n", &material.power);
		
		TryReadTextureName(materialData.diffuseMapName);
		TryReadTextureName(materialData.specularMapName);
		TryReadTextureName(materialData.bumpMapName);
		TryReadTextureName(materialData.normalMapName);
	}

	fclose(file);
}

void ModelIO::SaveSkeleton(std::filesystem::path filePath, const Model& model)
{
	if (!model.skeleton) {
		return;
	}

	filePath.replace_extension("skeleton");

	FILE* file = nullptr;
	auto err = fopen_s(&file, filePath.u8string().c_str(), "w");
	if (err != 0 || file == nullptr) {
		return;
	}

	fprintf(file, "BoneCount: %d\n", static_cast<uint32_t>(model.skeleton->bones.size()));
	for (auto& bone : model.skeleton->bones)
	{
		fprintf(file, "Name: %s\n", bone->name.c_str());
		fprintf(file, "Index: %d\n", bone->index);
		fprintf(file, "ParentIndex: %d\n", bone->parent ? bone->parent->index : -1);
		fprintf(file, "ChildCount: %d\n", static_cast<uint32_t>(bone->children.size()));
		if (!bone->children.empty())
		{
			for (auto child : bone->children) {
				fprintf(file, "%d ", child->index);
			}
			fprintf(file, "\n");
		}
		WriteMatrix(file, bone->toParentTransform);
		WriteMatrix(file, bone->offSetTransform);
	}
	fclose(file);
}

void ModelIO::LoadSkeleton(std::filesystem::path filePath, Model& model)
{
	filePath.replace_extension("skeleton");
	FILE* file = nullptr;
	auto err = fopen_s(&file, filePath.u8string().c_str(), "r");
	if (err != 0 || file == nullptr) {
		printf("Error: failed to open file %s for loading.", filePath.u8string().c_str());
		return;
	}

	model.skeleton = std::make_unique<Skeleton>();

	uint32_t boneCount = 0;
	fscanf_s(file, "BoneCount: %d\n", &boneCount);
	model.skeleton->bones.reserve(boneCount);

	for (uint32_t i = 0; i < boneCount; ++i)
	{
		auto& bone = model.skeleton->bones.emplace_back(std::make_unique<Bone>());

		char buffer[256];
		fscanf_s(file, "Name: %s\n", buffer, (uint32_t)std::size(buffer));
		bone->name = buffer;

		fscanf_s(file, "Index: %d\n", &bone->index);
		fscanf_s(file, "ParentIndex: %d\n", &bone->parentIndex);

		uint32_t childCount = 0;
		fscanf_s(file, "ChildCount: %d\n", &childCount);
		bone->childIndices.reserve(childCount);

		if (childCount > 0)
		{
			bone->childIndices.resize(childCount);
			for (uint32_t childIndex = 0; childIndex < childCount; ++childIndex)
			{
				fscanf_s(file, "%d ", &bone->childIndices[childIndex]);
			}
			fscanf_s(file, "\n");
		}
		ReadMatrix(file, bone->toParentTransform);
		ReadMatrix(file, bone->offSetTransform);
	}
	fclose(file);

	// Relink skeleton
	for (auto& bone : model.skeleton->bones)
	{
		if (bone->parentIndex != -1) {
			bone->parent = model.skeleton->bones[bone->parentIndex].get();
		}
		else {
			model.skeleton->root = bone.get();
		}

		bone->children.reserve(bone->childIndices.size());
		for (auto childIndex : bone->childIndices) {
			bone->children.push_back(model.skeleton->bones[childIndex].get());
		}
	}
}

void ModelIO::SaveAnimationSet(std::filesystem::path filePath, const Model& model)
{
	if (model.animationSet.empty()) {
		return;
	}

	filePath.replace_extension("animset");

	FILE* file = nullptr;
	auto err = fopen_s(&file, filePath.u8string().c_str(), "w");
	if (err != 0 || file == nullptr) {
		return;
	}

	const uint32_t clipCount = static_cast<uint32_t>(model.animationSet.size());
	fprintf(file, "ClipCount: %d\n", clipCount);

	for (auto& animationClip : model.animationSet)
	{
		fprintf(file, "Name: %s\n", animationClip.name.c_str());
		fprintf(file, "Duration: %f\n", animationClip.tickDuration);
		fprintf(file, "TicksPerSecond: %f\n", animationClip.ticksPerSecond);

		fprintf(file, "AnimationCount: %d\n", static_cast<uint32_t>(animationClip.boneAnimations.size()));
		if (!animationClip.boneAnimations.empty())
		{
			for (auto& animation : animationClip.boneAnimations)
			{
				if (animation)
				{
					fprintf(file, "[Animation]\n");
					AnimationIO::Write(file, *animation);
				}
				else
				{
					fprintf(file, "[Empty]\n");
				}
			}
		}
	}

	fclose(file);
}

void ModelIO::LoadAnimationSet(std::filesystem::path filePath, Model& model)
{
	filePath.replace_extension("animset");

	FILE* file = nullptr;
	auto err = fopen_s(&file, filePath.u8string().c_str(), "r");
	if (err != 0 || file == nullptr) {
		printf("Error: failed to open file %s for loading.", filePath.u8string().c_str());
		return;
	}

	uint32_t clipCount = 0;
	fscanf_s(file, "ClipCount: %u\n", &clipCount);

	// +clipCount because we want to append existing set
	model.animationSet.reserve(model.animationSet.size() + clipCount);
	for (uint32_t i = 0; i < clipCount; ++i)
	{
		auto& animationClip = model.animationSet.emplace_back();

		char buffer[256] = {0};
		int res = fscanf_s(file, "Name: %s\n", buffer, (uint32_t)std::size(buffer));
		if (res <= 0) {
			return;
		}

		animationClip.name = buffer;

		fscanf_s(file, "Duration: %f\n", &animationClip.tickDuration);
		fscanf_s(file, "TicksPerSecond: %f\n", &animationClip.ticksPerSecond);

		uint32_t animationCount = 0;
		fscanf_s(file, "AnimationCount: %d\n", &animationCount);
		if (animationCount > 0)
		{
			animationClip.boneAnimations.resize(animationCount);
			for (auto& animation : animationClip.boneAnimations)
			{
				fscanf_s(file, "%s\n", buffer, (uint32_t)std::size(buffer));
				if (strcmp(buffer, "[Animation]") == 0)
				{
					animation = std::make_unique<Animation>();
					AnimationIO::Read(file, *animation);
				}
			}
		}
	}

	fclose(file);
}