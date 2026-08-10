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

//IntersectData BoundingCapsule::IntersectBoundingCapsule(const BoundingBox& other)// const
//{
//    const Vector3 min = other.GetMinExtend();
//    const Vector3 max = other.GetMaxExtend();
//
//    // 1. Closest point on capsule's inner segment to the box
//    const Vector3 center = ClosestPointSegmentToBox(GetInnerTopCenter(), GetInnerBottomCenter(), min, max);
//
//    // 2. Closest point on AABB to that point (same as your sphere code)
//    Vector3 closest;
//    closest.x = std::max(min.x, std::min(center.x, max.x));
//    closest.y = std::max(min.y, std::min(center.y, max.y));
//    closest.z = std::max(min.z, std::min(center.z, max.z));
//
//    // 3. Vector from box -> capsule segment point
//    Vector3 delta = center - closest;
//
//	const float distSq = MagnitudeSqr(delta);
//	const float radiusSq = mRadius * mRadius;
//	if (distSq > radiusSq) // No intersection
//	{
//		return IntersectData();
//	}
//
//    Vector3 normal;
//    float penetration = 0.0f;
//
//    if (distSq > 0.0f)
//    {
//        // Center is outside the box (but within radius) - normal case
//        float dist = std::sqrt(distSq);
//        normal = delta / dist;
//        penetration = mRadius - dist;
//    }
//    else
//    {
//        // Sphere center is inside the box.
//        // Push out along the axis of least penetration.
//        Vector3 dMin = center - min;
//        Vector3 dMax = max - center;
//
//        float px = std::min(dMin.x, dMax.x);
//        float py = std::min(dMin.y, dMax.y);
//        float pz = std::min(dMin.z, dMax.z);
//
//        if (px <= py && px <= pz)
//        {
//            normal = Vector3((dMin.x < dMax.x) ? -1.0f : 1.0f, 0.0f, 0.0f);
//            penetration = mRadius + px;
//        }
//        else if (py <= px && py <= pz)
//        {
//            normal = Vector3(0.0f, (dMin.y < dMax.y) ? -1.0f : 1.0f, 0.0f);
//            penetration = mRadius + py;
//        }
//        else
//        {
//            normal = Vector3(0.0f, 0.0f, (dMin.z < dMax.z) ? -1.0f : 1.0f);
//            penetration = mRadius + pz;
//        }
//    }
//
//    return IntersectData(true, normal, {}, penetration);
//}

IntersectData BoundingCapsule::IntersectBoundingCapsule(const BoundingBox& other) //const
{
    // Box's world transform
    const Vector3 boxCenter = other.GetCenter();
    const Matrix4 boxRot = other.GetRotationMatrix4();     // columns = box local axes in world space
    const Matrix4 boxRotInv = Transpose(boxRot);           // inverse of a rotation matrix = its transpose
    const Vector3 halfExtents = other.GetExtend();         // local-space half extents

    // Transform capsule segment endpoints into box-local space
    const Vector3 topWorld = GetInnerTopCenter();
    const Vector3 bottomWorld = GetInnerBottomCenter();

    const Vector3 topLocal = (topWorld - boxCenter) * boxRotInv;
    const Vector3 bottomLocal = (bottomWorld - boxCenter) * boxRotInv;

    // Local-space box is now a simple AABB: [-halfExtents, +halfExtents]
    const Vector3 min = -halfExtents;
    const Vector3 max = halfExtents;

    // 1. Closest point on capsule's inner segment to the box (in LOCAL space)
    const Vector3 center = ClosestPointSegmentToBox(topLocal, bottomLocal, min, max);

    // 2. Closest point on AABB to that point (in LOCAL space)
    Vector3 closest;
    closest.x = std::max(min.x, std::min(center.x, max.x));
    closest.y = std::max(min.y, std::min(center.y, max.y));
    closest.z = std::max(min.z, std::min(center.z, max.z));

    // 3. Vector from box -> capsule segment point (still LOCAL space)
    Vector3 delta = center - closest;

    const float distSq = MagnitudeSqr(delta);
    const float radiusSq = mRadius * mRadius;
    if (distSq > radiusSq) // No intersection
    {
        return IntersectData();
    }

    Vector3 normalLocal;
    float penetration = 0.0f;

    if (distSq > 0.0f)
    {
        float dist = std::sqrt(distSq);
        normalLocal = delta / dist;
        penetration = mRadius - dist;
    }
    else
    {
        Vector3 dMin = center - min;
        Vector3 dMax = max - center;

        float px = std::min(dMin.x, dMax.x);
        float py = std::min(dMin.y, dMax.y);
        float pz = std::min(dMin.z, dMax.z);

        if (px <= py && px <= pz)
        {
            normalLocal = Vector3((dMin.x < dMax.x) ? -1.0f : 1.0f, 0.0f, 0.0f);
            penetration = mRadius + px;
        }
        else if (py <= px && py <= pz)
        {
            normalLocal = Vector3(0.0f, (dMin.y < dMax.y) ? -1.0f : 1.0f, 0.0f);
            penetration = mRadius + py;
        }
        else
        {
            normalLocal = Vector3(0.0f, 0.0f, (dMin.z < dMax.z) ? -1.0f : 1.0f);
            penetration = mRadius + pz;
        }
    }

    // 4. Transform the normal back into WORLD space before returning it
    const Vector3 normalWorld = normalLocal * boxRot; // rotation only, no translation needed for a direction

    return IntersectData(true, normalWorld, {}, penetration);
}

IntersectData BoundingCapsule::IntersectBoundingCapsule(const BoundingCapsule& other)
{
    Vector3 c1, c2;
    const float distSq = ClosestPointSegmentSegment(
        GetInnerTopCenter(), GetInnerBottomCenter(),
        other.GetInnerTopCenter(), other.GetInnerBottomCenter(),
        c1, c2);

    const float radiusSum = mRadius + other.mRadius;
    const float radiusSumSq = radiusSum * radiusSum;

    if (distSq > radiusSumSq) // No intersection
    {
        return IntersectData();
    }

    Vector3 normal;
    float penetration = 0.0f;

    if (distSq > 0.0f)
    {
        const float dist = std::sqrt(distSq);
        normal = (c1 - c2) / dist; // points from B's segment toward A's segment
        penetration = radiusSum - dist;
    }
    else
    {
        // Segments intersect exactly (distSq == 0) — pick a fallback normal.
        // This is rare in practice (would need coplanar crossing segments),
        // but must be handled to avoid a zero-length normal.
        normal = Vector3(0.0f, 1.0f, 0.0f); // arbitrary fallback, see note below
        penetration = radiusSum;
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

// Computes closest points between segment (p1,q1) and (p2,q2)
// Returns closest point on each segment via out params, and the squared distance
float BoundingCapsule::ClosestPointSegmentSegment(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, Vector3& c1, Vector3& c2)
{
    const Vector3 d1 = q1 - p1; // direction of segment 1
    const Vector3 d2 = q2 - p2; // direction of segment 2
    const Vector3 r = p1 - p2;

    const float a = Dot(d1, d1); // squared length of segment 1
    const float e = Dot(d2, d2); // squared length of segment 2
    const float f = Dot(d2, r);

    float s, t;
    const float EPSILON = 1e-6f;

    if (a <= EPSILON && e <= EPSILON)
    {
        // Both segments degenerate into points
        c1 = p1;
        c2 = p2;
        return MagnitudeSqr(c1 - c2);
    }

    if (a <= EPSILON)
    {
        // Segment 1 is a point
        s = 0.0f;
        t = std::clamp(f / e, 0.0f, 1.0f);
    }
    else
    {
        const float c = Dot(d1, r);

        if (e <= EPSILON)
        {
            // Segment 2 is a point
            t = 0.0f;
            s = std::clamp(-c / a, 0.0f, 1.0f);
        }
        else
        {
            const float b = Dot(d1, d2);
            const float denom = a * e - b * b;

            if (denom != 0.0f)
            {
                s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }
            else
            {
                // Segments are parallel — pick arbitrary s
                s = 0.0f;
            }

            t = (b * s + f) / e;

            // If t is out of range, clamp and recompute s
            if (t < 0.0f)
            {
                t = 0.0f;
                s = std::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
                s = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
    return MagnitudeSqr(c1 - c2);
}