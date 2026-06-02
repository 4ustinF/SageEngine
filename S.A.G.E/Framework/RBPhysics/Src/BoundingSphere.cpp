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

IntersectData BoundingSphere::IntersectBoundingSphere(const BoundingSphere& other) const
{
	const float radiusDistance = mRadius + other.GetRadius();
	Vector3 direction = (other.GetCenter() - mCenter);
	const float centerDistance = Math::Magnitude(direction);
	direction /= centerDistance;

	const float distance = centerDistance - radiusDistance;
	return IntersectData(distance < 0, direction * distance);
}

IntersectData BoundingSphere::IntersectBoundingSphere(const BoundingBox& other) const
{
	//Matrix3 rot = other.GetOrientation().ToMatrix3();

	//Vector3 axes[3] = {
	//	rot.GetColumn(0),
	//	rot.GetColumn(1),
	//	rot.GetColumn(2)
	//};

	//Vector3 closest = ClosestPointOnOBB(mCenter, other.GetCenter(), axes, other.GetExtend());

	//Vector3 diff = mCenter - closest;
	//return IntersectData(Dot(diff, diff) <= GetRadiusSquared(), {});


	return IntersectData(false, {});
}

