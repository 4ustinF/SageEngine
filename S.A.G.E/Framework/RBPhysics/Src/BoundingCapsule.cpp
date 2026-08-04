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
