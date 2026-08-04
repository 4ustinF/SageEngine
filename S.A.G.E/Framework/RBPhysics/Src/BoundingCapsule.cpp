#include "Precompiled.h"
#include "BoundingCapsule.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;
using namespace SAGE::Graphics;

void BoundingCapsule::DebugDraw(SAGE::Math::Quaternion orientation, bool fillDebugShapes)
{
	if (fillDebugShapes)
	{
		SimpleDraw::AddCapsule(mCenter, 32, 16, mRadius, mHeight, mOrientation, mDebugColor); // TODO:
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

Vector3 BoundingCapsule::GetClosestPoint(const Collider& otherCollider) const
{
	return mCenter - (Normalize(mCenter - otherCollider.GetCenter()) * mRadius);
}
