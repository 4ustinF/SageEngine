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

    // 1. Closest point on capsule's inner segment to the box
    const Vector3 center = ClosestPointSegmentToBox(GetInnerTopCenter(), GetInnerBottomCenter(), min, max);

    // 2. Closest point on AABB to that point (same as your sphere code)
    Vector3 closest;
    closest.x = std::max(min.x, std::min(center.x, max.x));
    closest.y = std::max(min.y, std::min(center.y, max.y));
    closest.z = std::max(min.z, std::min(center.z, max.z));

    // 3. Vector from box -> capsule segment point
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

Vector3 BoundingCapsule::ClosestPointOnSegment(const Vector3& a, const Vector3& b, const Vector3& point)
{
    Vector3 ab = b - a;
    float abLenSq = MagnitudeSqr(ab);

    if (abLenSq < 0.0001f) // degenerate segment (a == b), treat as a point
        return a;

    float t = Dot(point - a, ab) / abLenSq;
    t = std::max(0.0f, std::min(1.0f, t)); // clamp to segment

    return a + ab * t;
}

Vector3 BoundingCapsule::ClosestPointSegmentToBox(const Vector3& segA, const Vector3& segB, const Vector3& min, const Vector3& max)
{
    // Start with a guess: segment midpoint, clamped into the box
    Vector3 boxPoint = (segA + segB) * 0.5f;
    boxPoint.x = std::max(min.x, std::min(boxPoint.x, max.x));
    boxPoint.y = std::max(min.y, std::min(boxPoint.y, max.y));
    boxPoint.z = std::max(min.z, std::min(boxPoint.z, max.z));

    Vector3 segPoint;

    // A couple of iterations converges very quickly for segment-vs-AABB
    for (int i = 0; i < 2; ++i)
    {
        segPoint = ClosestPointOnSegment(segA, segB, boxPoint);

        boxPoint.x = std::max(min.x, std::min(segPoint.x, max.x));
        boxPoint.y = std::max(min.y, std::min(segPoint.y, max.y));
        boxPoint.z = std::max(min.z, std::min(segPoint.z, max.z));
    }

    return segPoint; // this is your sphere center
}