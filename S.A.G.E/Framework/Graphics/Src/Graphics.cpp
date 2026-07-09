#include "Precompiled.h"
#include "Graphics.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

bool SAGE::Graphics::IntersectRayMesh(const Ray& ray, const Mesh& mesh, RayHit& outHit) // TODO: -_-
{
	bool hasHit = false;
	float closestDistance = FLT_MAX;

	for (uint32_t i = 0; i + 2 < mesh.indices.size(); i += 3)
	{
		const uint32_t i0 = mesh.indices[i + 0];
		const uint32_t i1 = mesh.indices[i + 1];
		const uint32_t i2 = mesh.indices[i + 2];

		const Math::Vector3& v0 = mesh.vertices[i0].position;
		const Math::Vector3& v1 = mesh.vertices[i1].position;
		const Math::Vector3& v2 = mesh.vertices[i2].position;

		float distance = 0.0f;
		Math::Vector3 normal;

		if (IntersectRayTriangle(ray, v0, v1, v2, distance, normal))
		{
			if (distance < closestDistance)
			{
				closestDistance = distance;

				outHit.hit = true;
				outHit.distance = distance;
				outHit.position = ray.origin + ray.direction * distance;
				outHit.normal = normal;

				hasHit = true;
			}
		}
	}

	return hasHit;
}
