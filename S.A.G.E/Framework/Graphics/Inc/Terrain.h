#pragma once

#include "MeshTypes.h"

namespace SAGE::Graphics
{
	class Terrain final
	{
	public:
		void Initialize(std::filesystem::path fileName, float heightScale);

		Mesh mesh;

		size_t mRows = 0;
		size_t mColumns = 0;
	};
}