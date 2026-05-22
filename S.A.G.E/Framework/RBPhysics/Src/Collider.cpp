#include "Precompiled.h"
#include "Collider.h"

#include "IntersectData.h"
#include "BoundingSphere.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;

IntersectData Collider::Intersect(const Collider& other) const
{
	if (mType == TYPE_SPHERE && other.GetType() == TYPE_SPHERE)
	{
		BoundingSphere* self = (BoundingSphere*)this; // TODO: Safer cast?
		return self->IntersectBoundingSphere((BoundingSphere&)other); // TODO: Better cast?
	}

	return IntersectData(false, Vector3::Zero); // TODO: Should never reach this add error logs if we do.
}