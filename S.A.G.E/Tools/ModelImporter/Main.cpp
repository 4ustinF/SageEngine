#include <SAGE/Inc/SAGE.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cstdio>

using namespace SAGE::Graphics;
using namespace SAGE::Math;

using BoneIndexLookup = std::map<std::string, int>; // Used to lookup bone index by name

struct Arguments
{
	std::filesystem::path inputFileName;
	std::filesystem::path outputFileName;
	float scale = 1.0f;
	float animOnly = false;
};

std::optional<Arguments> ParseArgs(int argc, char* argv[])
{
	if (argc < 3) { return std::nullopt; }

	Arguments arguments;
	arguments.inputFileName = argv[argc - 2];
	arguments.outputFileName = argv[argc - 1];

	for (int i = 1; i + 2 < argc; ++i) {
		if (strcmp(argv[i], "-scale") == 0) {
			arguments.scale = static_cast<float>(atof(argv[i + 1]));
			++i;
		}
		if (strcmp(argv[i], "-animOnly") == 0) {
			arguments.animOnly = true;
		}
	}

	return arguments;
}

void PrintUsage()
{
	printf
	(
		"== ModelImporter Help == \n"
		"\n"
		"Usage\n"
		"	ModelImporter.exe [Options] <InputFile> <OutputFile>\n"
		"Options:\n"
		"	-scale <number>	Scale to apply to the model.\n"
		"\n"
	);
}

Color ToColor(const aiColor3D& c)
{
	return {
		static_cast<float>(c.r),
		static_cast<float>(c.g),
		static_cast<float>(c.b),
		1.0f
	};
}

Vector3 ToVector3(const aiVector3D& v)
{
	return {
		static_cast<float>(v.x),
		static_cast<float>(v.y),
		static_cast<float>(v.z)
	};
}

Quaternion ToQuaternion(const aiQuaternion& q)
{
	return
	{
		static_cast<float>(q.w),
		static_cast<float>(q.x),
		static_cast<float>(q.y),
		static_cast<float>(q.z)
	};
}

Matrix4 ToMatrix4(const aiMatrix4x4& m)
{
	// Take the tranpose from assimp matrix
	return{
		static_cast<float>(m.a1),
		static_cast<float>(m.b1),
		static_cast<float>(m.c1),
		static_cast<float>(m.d1),

		static_cast<float>(m.a2),
		static_cast<float>(m.b2),
		static_cast<float>(m.c2),
		static_cast<float>(m.d2),

		static_cast<float>(m.a3),
		static_cast<float>(m.b3),
		static_cast<float>(m.c3),
		static_cast<float>(m.d3),

		static_cast<float>(m.a4),
		static_cast<float>(m.b4),
		static_cast<float>(m.c4),
		static_cast<float>(m.d4)
	};
}

void ExportEmbeddedTexture(const aiTexture* texture, const Arguments& args, const std::filesystem::path& fileName)
{
	printf("Extracting embedded texture %s\n", fileName.u8string().c_str());

	std::string fullFileName = args.outputFileName.string();
	fullFileName = fullFileName.substr(0, fullFileName.rfind('/') + 1);
	fullFileName += fileName.filename().u8string().c_str();

	FILE* file = nullptr;
	auto err = fopen_s(&file, fullFileName.c_str(), "wb");
	if (err != 0 || file == nullptr)
	{
		printf("Error: failed to open file %s for saving.\n", fullFileName.c_str());
		return;
	}

	size_t written = fwrite(texture->pcData, 1, texture->mWidth, file);
	ASSERT(written == texture->mWidth, "Error: Failed to extract embedded texture!");
	fclose(file);
}

std::string FindTexture(const aiScene* scene, const aiMaterial* aiMaterial, aiTextureType textureType, const Arguments& args, const char* suffix, uint32_t materialIndex)
{
	std::filesystem::path textureName;

	const uint32_t textureCount = aiMaterial->GetTextureCount(textureType);
	if (textureCount > 0)
	{
		aiString texturePath;
		if (aiMaterial->GetTexture(textureType, 0, &texturePath) == aiReturn_SUCCESS)
		{
			// if texture starts with *, then the texture is embedded.
			// https://github.com/assimp/assimp/wiki/Embedded-Textures-References
			if (texturePath.C_Str()[0] == '*')
			{
				std::string fileName = args.inputFileName.string();
				fileName.erase(fileName.length() - 4); // remove ".fbx" extension
				fileName += suffix;
				fileName += texturePath.C_Str()[1];

				ASSERT(scene->HasTextures(), "Error: No embedded texture found!");

				int textureIndex = atoi(texturePath.C_Str() + 1);
				ASSERT(textureIndex < (int)scene->mNumTextures, "Error: Invalid texture index!");

				const aiTexture* embeddedTexture = scene->mTextures[textureIndex];
				ASSERT(embeddedTexture->mHeight == 0, "Error: Uncompressed texture found!");

				if (embeddedTexture->CheckFormat("jpg")) {
					fileName += ".jpg";
				}
				else if (embeddedTexture->CheckFormat("png")) {
					fileName += ".png";
				}
				else {
					ASSERT(embeddedTexture->mHeight == 0, "Error: Unrecoginized texture format!");
				}

				ExportEmbeddedTexture(embeddedTexture, args, fileName);

				printf("Adding texture %s\n", fileName.c_str());
				textureName = fileName;
			}
			else if (auto embeddedTexture = scene->GetEmbeddedTexture(texturePath.C_Str()); embeddedTexture)
			{
				// For FBX files, embedded textures is now accessed using GetEmbeddedTexture
				// https://github.com/assimp/assimp/issues/1830

				std::filesystem::path embeddedFilePath = texturePath.C_Str();
				std::string fileName = args.inputFileName.string();
				fileName.erase(fileName.length() - 4); // remove ".fbx" extension
				fileName += suffix;
				fileName += "_" + std::to_string(materialIndex);
				fileName += embeddedFilePath.extension().u8string();

				ExportEmbeddedTexture(embeddedTexture, args, fileName);
				textureName = fileName;
			}
			else
			{
				std::filesystem::path filePath = texturePath.C_Str();
				std::string fileName = filePath.filename().u8string();
				printf("Adding texture %s\n", fileName.c_str());
				textureName = fileName;
			}
		}
	}
	return textureName.filename().u8string().c_str();
}

// Check if aiBone exist in skeleton, if so just return the index.
// Otherwise, add it to the skeleton. The aiBone must have a name!
int TryAddBone(const aiBone* aiBone, Skeleton& skeleton, BoneIndexLookup& boneIndexLookup)
{
	std::string name = aiBone->mName.C_Str();
	ASSERT(!name.empty(), "Error: aiBone has no name!");

	auto iter = boneIndexLookup.find(name);
	if (iter != boneIndexLookup.end()) {
		return iter->second;
	}

	// Add a new bone in the skeleton for this
	auto& newBone = skeleton.bones.emplace_back(std::make_unique<Bone>());
	newBone->name = std::move(name);
	newBone->index = static_cast<int>(skeleton.bones.size()) - 1;
	newBone->offSetTransform = ToMatrix4(aiBone->mOffsetMatrix);

	// Cache the bone index
	boneIndexLookup.emplace(newBone->name, newBone->index);
	return newBone->index;
}

// Recursivly 
Bone* BuildSkeleton(const aiNode& sceneNode, Bone* parent, Skeleton& skeleton, BoneIndexLookup& boneIndexLookup)
{
	Bone* bone = nullptr;

	std::string boneName = sceneNode.mName.C_Str();
	auto iter = boneIndexLookup.find(boneName);
	if (iter != boneIndexLookup.end())
	{
		// Bone already added to skeleton
		bone = skeleton.bones[iter->second].get();
	}
	else
	{
		// Add a new bone in the skeleton for this (possible need to generate a name for it)
		bone = skeleton.bones.emplace_back(std::make_unique<Bone>()).get();
		bone->index = static_cast<int>(skeleton.bones.size()) - 1;
		bone->offSetTransform = Matrix4::Identity;
		if (boneName.empty()) {
			bone->name = "NoName" + std::to_string(bone->index);
		}
		else {
			bone->name = std::move(boneName);
		}

		// Cache the bone index
		boneIndexLookup.emplace(bone->name, bone->index);
	}

	// Link to your parent
	bone->parent = parent;
	bone->parentIndex = parent == nullptr ? -1 : parent->index;
	bone->toParentTransform = ToMatrix4(sceneNode.mTransformation);

	// Recursive throuh your children
	bone->children.reserve(sceneNode.mNumChildren);
	for (uint32_t i = 0; i < sceneNode.mNumChildren; ++i)
	{
		Bone* child = BuildSkeleton(*sceneNode.mChildren[i], bone, skeleton, boneIndexLookup);
		bone->children.push_back(child);
		bone->childIndices.push_back(child->index);
	}
	return bone;
}

int main(int argc, char* argv[])
{
	const auto argOpt = ParseArgs(argc, argv);
	if (!argOpt.has_value()) {
		PrintUsage();
		return -1;
	}

	const auto& arguments = argOpt.value();

	const uint32_t flags = aiProcessPreset_TargetRealtime_Quality | 
		/*aiProcess_PreTransformVertices | */
		aiProcess_ConvertToLeftHanded;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(arguments.inputFileName.u8string().c_str(), flags);
	if (scene == nullptr) {
		printf("Error: %s", importer.GetErrorString());
		return -1;
	}

	// scene
	// +- mesh[_][_][_][_][_][_]...
	// +- material[_][_][_][_][_]...
	// +- animation[_][_][_][_][_]...
	// +- mRootNode
	//     +-Node
	//       +- Node
	//       +- Node
	//          +- Node ...
	//     +-Node
	//       +- Node ...
	//     +-Node
	//     ...

	printf(
		"--------------------------------------------\n"
		"Importing %s...\n",
		arguments.inputFileName.u8string().c_str()
	);

	Model model;
	BoneIndexLookup boneIndexLookup;

	// Look for mesh data
	if (scene->HasMeshes())
	{
		printf("Reading mesh data...\n");

		const uint32_t numMeshes = scene->mNumMeshes;
		model.meshData.reserve(numMeshes);

		for (uint32_t meshIndex = 0; meshIndex < numMeshes; ++meshIndex) {
			const auto aiMesh = scene->mMeshes[meshIndex];

			// Ignore anything that is not a triangle mesh for now (e.g. skipping points and lines)
			if (aiMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
				continue;
			}

			const uint32_t numVertices = aiMesh->mNumVertices;
			const uint32_t numFaces = aiMesh->mNumFaces;
			const uint32_t numIndices = numFaces * 3;

			auto& meshData = model.meshData.emplace_back();

			printf("Reading material index...\n");

			meshData.materialIndex = aiMesh->mMaterialIndex;

			printf("Reading vertices...\n");

			auto& mesh = meshData.mesh;
			mesh.vertices.reserve(numVertices);

			const aiVector3D* positions = aiMesh->mVertices;
			const aiVector3D* normals = aiMesh->mNormals;
			const aiVector3D* tangents = aiMesh->HasTangentsAndBitangents() ? aiMesh->mTangents : nullptr;
			const aiVector3D* texCoords = aiMesh->HasTextureCoords(0) ? aiMesh->mTextureCoords[0] : nullptr;
			for (uint32_t i = 0; i < numVertices; ++i)
			{
				auto& vertex = mesh.vertices.emplace_back();
				vertex.position = ToVector3(positions[i]) * arguments.scale;
				vertex.normal = ToVector3(normals[i]);
				vertex.tangent = tangents ? ToVector3(tangents[i]) : Vector3::Zero;
				if (texCoords) {
					vertex.uv.x = texCoords[i].x;
					vertex.uv.y = texCoords[i].y;
				}
			}

			printf("Reading indices...\n");

			mesh.indices.reserve(numIndices);

			const auto aiFaces = aiMesh->mFaces;
			for (uint32_t i = 0; i < numFaces; ++i) {
				const auto& aiFace = aiFaces[i];
				mesh.indices.push_back(aiFace.mIndices[0]);
				mesh.indices.push_back(aiFace.mIndices[1]);
				mesh.indices.push_back(aiFace.mIndices[2]);
			}

			if (aiMesh->HasBones())
			{
				if (!model.skeleton) {
					model.skeleton = std::make_unique<Skeleton>();
				}

				// Tack how many weights have we added to each vertex so far
				std::vector<int> numWeightsAdded(mesh.vertices.size(), 0);

				printf("Reading bone weights...\n");
				for (uint32_t meshBoneIndex = 0; meshBoneIndex < aiMesh->mNumBones; ++meshBoneIndex)
				{
					const auto aiBone = aiMesh->mBones[meshBoneIndex];
					const int boneIndex = TryAddBone(aiBone, *(model.skeleton), boneIndexLookup);

					for (uint32_t weightIndex = 0; weightIndex < aiBone->mNumWeights; ++weightIndex)
					{
						const aiVertexWeight& weight = aiBone->mWeights[weightIndex];
						auto& vertex = mesh.vertices[weight.mVertexId];
						auto& count = numWeightsAdded[weight.mVertexId];

						// SAGE vertices default can hold up to 4 weights
						if (count < Vertex::MaxBoneWeights)
						{
							vertex.boneIndices[count] = boneIndex;
							vertex.boneWeights[count] = weight.mWeight;
							++count;
						}
						else {
							printf("Losing out on bone weights\n");
						}
					}
				}
			}
		}
	}

	// Look for material data
	if (!arguments.animOnly && scene->HasMaterials())
	{
		printf("Reading materials...\n");

		const uint32_t numMaterials = scene->mNumMaterials;
		model.materialData.reserve(numMaterials);

		for (uint32_t materialIndex = 0; materialIndex < numMaterials; ++materialIndex)
		{
			const auto aiMaterial = scene->mMaterials[materialIndex];

			aiColor3D ambient, diffuse, specular;
			ai_real specularPower = 1.0f;

			aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			aiMaterial->Get(AI_MATKEY_SHININESS, specularPower);

			auto& materialData = model.materialData.emplace_back();
			materialData.material.ambient = ToColor(ambient);
			materialData.material.diffuse = ToColor(diffuse);
			materialData.material.specular = ToColor(specular);
			materialData.material.power = static_cast<float>(specularPower);
			materialData.diffuseMapName = FindTexture(scene, aiMaterial, aiTextureType_DIFFUSE, arguments, "_diffuse", materialIndex);
			materialData.specularMapName = FindTexture(scene, aiMaterial, aiTextureType_SPECULAR, arguments, "_specular", materialIndex);
			//materialData.specularMapName = FindTexture(scene, aiMaterial, aiTextureType_SHININESS, arguments, "_specular", materialIndex);
			materialData.bumpMapName = FindTexture(scene, aiMaterial, aiTextureType_DISPLACEMENT, arguments, "_bump", materialIndex);
			materialData.normalMapName = FindTexture(scene, aiMaterial, aiTextureType_NORMALS, arguments, "_normal", materialIndex);
		}
	}

	// Look for skeleton data
	if (!boneIndexLookup.empty())
	{
		printf("Building skeleton...\n");

		BuildSkeleton(*scene->mRootNode, nullptr, *(model.skeleton), boneIndexLookup);

		// Apply scale
		for (auto& bone : model.skeleton->bones)
		{
			bone->offSetTransform._41 *= arguments.scale;
			bone->offSetTransform._42 *= arguments.scale;
			bone->offSetTransform._43 *= arguments.scale;
			bone->toParentTransform._41 *= arguments.scale;
			bone->toParentTransform._42 *= arguments.scale;
			bone->toParentTransform._43 *= arguments.scale;
		}
	}

	// Look for animation data
	if (scene->HasAnimations())
	{
		printf("Reading animations...\n");

		for (uint32_t animIndex = 0; animIndex < scene->mNumAnimations; ++animIndex)
		{
			const aiAnimation* inputAnim = scene->mAnimations[animIndex];
			AnimationClip animClip;

			if (inputAnim->mName.length > 0) {
				animClip.name = inputAnim->mName.C_Str();
			}
			else {
				animClip.name = "Anim" + std::to_string(animIndex);
			}

			animClip.tickDuration = static_cast<float>(inputAnim->mDuration);
			animClip.ticksPerSecond = static_cast<float>(inputAnim->mTicksPerSecond);

			printf("Reading bone animations for %s...\n", animClip.name.c_str());

			// Reserve space so we have one animation slot per bone, note that not
			// all bones will have animations so some slots will stay empty. However
			// keeping them the same size means we can use bone index directly to look up animations
			animClip.boneAnimations.resize(model.skeleton->bones.size());
			for (uint32_t boneAnimIndex = 0; boneAnimIndex < inputAnim->mNumChannels; ++boneAnimIndex)
			{
				const aiNodeAnim* aiBoneAnim = inputAnim->mChannels[boneAnimIndex];
				int slotIndex = boneIndexLookup[aiBoneAnim->mNodeName.C_Str()];
				auto& boneAnim = animClip.boneAnimations[slotIndex];
				boneAnim = std::make_unique<Animation>();

				AnimationBuilder builder;
				for (uint32_t keyIndex = 0; keyIndex < aiBoneAnim->mNumPositionKeys; ++keyIndex)
				{
					auto& key = aiBoneAnim->mPositionKeys[keyIndex];
					builder.AddPositionKey(ToVector3(key.mValue) * arguments.scale, static_cast<float>(key.mTime));
				}

				for (uint32_t keyIndex = 0; keyIndex < aiBoneAnim->mNumRotationKeys; ++keyIndex)
				{
					auto& key = aiBoneAnim->mRotationKeys[keyIndex];
					builder.AddRotationKey(ToQuaternion(key.mValue), static_cast<float>(key.mTime));
				}

				for (uint32_t keyIndex = 0; keyIndex < aiBoneAnim->mNumScalingKeys; ++keyIndex)
				{
					auto& key = aiBoneAnim->mScalingKeys[keyIndex];
					builder.AddScaleKey(ToVector3(key.mValue), static_cast<float>(key.mTime));
				}

				*boneAnim = builder.Get();
			}
			model.animationSet.emplace_back(std::move(animClip));
		}
	}

	if (!arguments.animOnly)
	{
		ModelIO::SaveModel(arguments.outputFileName, model);
		ModelIO::SaveMaterial(arguments.outputFileName, model);
		ModelIO::SaveSkeleton(arguments.outputFileName, model);
	}
	ModelIO::SaveAnimationSet(arguments.outputFileName, model);

	printf("All Done\n");

	return 0;
}