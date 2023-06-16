#include "Precompiled.h"
#include "MeshUtil.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void MeshUtil::ComputeNormal(Mesh& mesh)
{
	std::vector<Math::Vector3> accumulatedNormals;
	accumulatedNormals.resize(mesh.vertices.size());
	for (uint32_t i = 0; i < mesh.indices.size(); i += 3)
	{
		const auto ia = mesh.indices[i + 0u];
		const auto ib = mesh.indices[i + 1u];
		const auto ic = mesh.indices[i + 2u];

		auto& va = mesh.vertices[ia];
		auto& vb = mesh.vertices[ib];
		auto& vc = mesh.vertices[ic];

		const auto ab = Math::Normalize(vb.position - va.position);
		const auto ac = Math::Normalize(vc.position - va.position);
		const auto normal = Math::Normalize(Math::Cross(ab, ac));

		accumulatedNormals[ia] += normal;
		accumulatedNormals[ib] += normal;
		accumulatedNormals[ic] += normal;
	}

	for (uint32_t i = 0; i < mesh.vertices.size(); ++i) {
		mesh.vertices[i].normal = Math::Normalize(accumulatedNormals[i]);
	}
}