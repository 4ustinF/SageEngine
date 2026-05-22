#include "Precompiled.h"
#include "BoundingSphere.h"

#include "IntersectData.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;

IntersectData BoundingSphere::IntersectBoundingSphere(const BoundingSphere& other) const
{
	const float radiusDistance = mRadius + other.GetRadius();
	Vector3 direction = (other.GetCenter() - mCenter);
	const float centerDistance = Math::Magnitude(direction);
	direction /= centerDistance;

	const float distance = centerDistance - radiusDistance;
	return IntersectData(distance < 0, direction * distance);
}

void BoundingSphere::Transform(const Math::Vector3& translation)
{
	mCenter += translation;
}
