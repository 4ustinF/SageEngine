#include "Precompiled.h"
#include "Terrain.h"

#include "MeshUtil.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void Terrain::Initialize(std::filesystem::path fileName, float heightScale)
{
	FILE* file = nullptr;
	fopen_s(&file, fileName.u8string().c_str(), "rb");

	fseek(file, 0L, SEEK_END);
	const size_t fileSize = ftell(file);
	const size_t dimension = (size_t)sqrt(static_cast<float>(fileSize));
	fseek(file, 0L, SEEK_SET);

	mRows = dimension;
	mColumns = dimension;

	const float tileCount = 20.0f;

	mesh.vertices.resize(mRows * mColumns);
	for (size_t z = 0; z < mRows; ++z)
	{
		for (size_t x = 0; x < mColumns; ++x)
		{
			const float height = fgetc(file) / 255.0f * heightScale;
			const size_t index = x + (z * mRows);

			auto& vetex = mesh.vertices[index];
			const float posX = static_cast<float>(x);
			const float posZ = static_cast<float>(z);
			vetex.position = Math::Vector3{ posX, height, posZ };
			//vetex.normal = Math::Vector3::YAxis;
			vetex.uv.x = posX / mColumns * tileCount;
			vetex.uv.y = posZ / mRows * tileCount;
		}
	}
	fclose(file);

	const size_t cells = (mRows - 1) * (mColumns - 1);
	mesh.indices.reserve(cells * 6);
	for (size_t z = 0; z < mRows - 1; ++z)
	{
		for (size_t x = 0; x < mColumns - 1; ++x)
		{
			const uint32_t bl = static_cast<uint32_t>((x + 0) + (z + 0) * mColumns);
			const uint32_t tl = static_cast<uint32_t>((x + 0) + (z + 1) * mColumns);
			const uint32_t br = static_cast<uint32_t>((x + 1) + (z + 0) * mColumns);
			const uint32_t tr = static_cast<uint32_t>((x + 1) + (z + 1) * mColumns);

			mesh.indices.push_back(bl);
			mesh.indices.push_back(tl);
			mesh.indices.push_back(tr);

			mesh.indices.push_back(bl);
			mesh.indices.push_back(tr);
			mesh.indices.push_back(br);
		}
	}

	MeshUtil meshUtil;
	meshUtil.ComputeNormal(mesh);
}
