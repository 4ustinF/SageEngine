#include "Precompiled.h"
#include "BoundingSphere.h"

#include "BoundingBox.h"
#include "IntersectData.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;
using namespace SAGE::Graphics;

void BoundingSphere::DebugDraw(SAGE::Math::Quaternion orientation, bool fillDebugShapes)
{
	if (fillDebugShapes)
	{
		SimpleDraw::AddFilledSphere(mCenter, mDebugDivisions, mRadius, mDebugColor);
	}
	else
	{
		SimpleDraw::AddSphere(mCenter, mDebugDivisions, mRadius, mDebugColor, orientation);
	}
}

void BoundingSphere::Transform(const Math::Vector3& translation)
{
	mCenter += translation;
}

Vector3 BoundingSphere::GetClosestPoint(const Collider& otherCollider) const
{
	return mCenter - (Normalize(mCenter - otherCollider.GetCenter()) * mRadius);
}

IntersectData BoundingSphere::IntersectBoundingSphere(const BoundingSphere& other) const
{
	const float radiusDistance = mRadius + other.GetRadius();
	Vector3 direction = (other.GetCenter() - mCenter);
	const float centerDistance = Math::Magnitude(direction);
	direction /= centerDistance;

	const float distance = centerDistance - radiusDistance;
	return IntersectData(distance < 0, direction * distance);
}

//IntersectData BoundingSphere::IntersectBoundingSphere(const BoundingBox& other) const
//{
//	bool doesIntersect = false;
//	const Vector3 min = other.GetMinExtend();
//	const Vector3 max = other.GetMaxExtend();
//	const Vector3 point = GetClosestPoint(other);
//
//	if (point.x < min.x || point.x > max.x ||
//		point.y < min.y || point.y > max.y ||
//		point.z < min.z || point.z > max.z)
//	{
//		doesIntersect = false;
//	}
//	else
//	{
//		doesIntersect = true;
//	}
//
//	return IntersectData(doesIntersect, {});
//}

IntersectData BoundingSphere::IntersectBoundingSphere(const BoundingBox& other) const
{
	const Vector3 min = other.GetMinExtend();
	const Vector3 max = other.GetMaxExtend();

	// 1. Closest point on AABB to sphere center
	Vector3 closest;
	closest.x = std::max(min.x, std::min(mCenter.x, max.x));
	closest.y = std::max(min.y, std::min(mCenter.y, max.y));
	closest.z = std::max(min.z, std::min(mCenter.z, max.z));

	// 2. Vector from box -> sphere
	Vector3 delta = mCenter - closest;

	float distSq = MagnitudeSqr(delta);
	float radiusSq = mRadius * mRadius;

	if (distSq > radiusSq)
	{
		// No intersection
		return IntersectData(false, {});
	}

	// 3. Compute penetration depth
	float dist = Sqr(distSq);

	float penetration;

	if (dist > 0.0f)
	{
		penetration = mRadius - dist;
	}
	else
	{
		// Sphere center is inside box
		// Push out along closest axis
		Vector3 dMin = mCenter - min;
		Vector3 dMax = max - mCenter;

		float px = std::min(dMin.x, dMax.x);
		float py = std::min(dMin.y, dMax.y);
		float pz = std::min(dMin.z, dMax.z);

		float minPen = px;
		if (py < minPen) minPen = py;
		if (pz < minPen) minPen = pz;

		penetration = mRadius + minPen;
	}

	return IntersectData(true, {});
}

