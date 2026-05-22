#include "Precompiled.h"
#include "BoundingSphere.h"

#include "IntersectData.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;

IntersectData BoundingSphere::IntersectBoundingSphere(const BoundingSphere& other)
{
	const float radiusDistance = mRadius + other.GetRadius();
	const float centerDistance = Distance(other.GetCenter(), mCenter);
	return IntersectData(centerDistance < radiusDistance, centerDistance - radiusDistance);
}