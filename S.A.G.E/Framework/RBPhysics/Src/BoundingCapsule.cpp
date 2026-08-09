#include "Precompiled.h"
#include "BoundingCapsule.h"

#include "BoundingBox.h"
#include "IntersectData.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;
using namespace SAGE::Graphics;

void BoundingCapsule::DebugDraw(Math::Quaternion orientation, bool fillDebugShapes)
{
	if (fillDebugShapes)
	{
		SimpleDraw::AddFilledCapsule(mCenter, 32, 16, mRadius, mHeight, mOrientation, mDebugColor);
	}
	else
	{
		SimpleDraw::AddCapsule(mCenter, 32, 16, mRadius, mHeight, mOrientation, mDebugColor);
	}
}

void BoundingCapsule::Transform(const Math::Vector3& translation)
{
	mCenter += translation;
}

IntersectData BoundingCapsule::IntersectBoundingCapsule(const BoundingBox& other)// const
{
	const Vector3 min = other.GetMinExtend();
	const Vector3 max = other.GetMaxExtend();
	const Vector3 center = GetInnerBottomCenter();

	SimpleDraw::AddSphere(center, 16, mRadius, mDebugColor);

	// 1. Closest point on AABB to sphere center
	Vector3 closest;
	closest.x = std::max(min.x, std::min(center.x, max.x));
	closest.y = std::max(min.y, std::min(center.y, max.y));
	closest.z = std::max(min.z, std::min(center.z, max.z));

	// 2. Vector from box -> sphere
	Vector3 delta = center - closest;

	const float distSq = MagnitudeSqr(delta);
	const float radiusSq = mRadius * mRadius;

	if (distSq > radiusSq) // No intersection
	{
		return IntersectData();
	}

    Vector3 normal;
    float penetration = 0.0f;

    if (distSq > 0.0f)
    {
        // Center is outside the box (but within radius) - normal case
        float dist = std::sqrt(distSq);
        normal = delta / dist;
        penetration = mRadius - dist;
    }
    else
    {
        // Sphere center is inside the box.
        // Push out along the axis of least penetration.
        Vector3 dMin = center - min;
        Vector3 dMax = max - center;

        float px = std::min(dMin.x, dMax.x);
        float py = std::min(dMin.y, dMax.y);
        float pz = std::min(dMin.z, dMax.z);

        if (px <= py && px <= pz)
        {
            normal = Vector3((dMin.x < dMax.x) ? -1.0f : 1.0f, 0.0f, 0.0f);
            penetration = mRadius + px;
        }
        else if (py <= px && py <= pz)
        {
            normal = Vector3(0.0f, (dMin.y < dMax.y) ? -1.0f : 1.0f, 0.0f);
            penetration = mRadius + py;
        }
        else
        {
            normal = Vector3(0.0f, 0.0f, (dMin.z < dMax.z) ? -1.0f : 1.0f);
            penetration = mRadius + pz;
        }
    }

    return IntersectData(true, normal, {}, penetration);
}
