#include "Precompiled.h"
#include "BoundingBox.h"

#include "IntersectData.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;
using namespace SAGE::Graphics;

void BoundingBox::DebugDraw(SAGE::Math::Quaternion orientation, bool fillDebugShapes)
{
	if (fillDebugShapes)
	{
		SimpleDraw::AddFilledOBB(mCenter, mExtend, orientation, mDebugColor);
	}
	else
	{
		SimpleDraw::AddOBB(mCenter, mExtend, orientation, mDebugColor);
	}
}

void BoundingBox::Transform(const Math::Vector3& translation)
{
	mCenter += translation;
}

//IntersectData BoundingBox::IntersectBoundingSphere(const BoundingSphere& other) const
//{
//	const float radiusDistance = mRadius + other.GetRadius();
//	Vector3 direction = (other.GetCenter() - mCenter);
//	const float centerDistance = Math::Magnitude(direction);
//	direction /= centerDistance;
//
//	const float distance = centerDistance - radiusDistance;
//	return IntersectData(distance < 0, direction * distance);
//}


