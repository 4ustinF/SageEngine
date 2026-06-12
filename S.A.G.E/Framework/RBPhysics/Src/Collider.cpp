#include "Precompiled.h"
#include "Collider.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "IntersectData.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;

IntersectData Collider::Intersect(const Collider& other) const
{
	if (mType == TYPE_SPHERE)
	{
		BoundingSphere* self = (BoundingSphere*)this; // TODO: Safer cast?

		if (other.GetType() == TYPE_SPHERE)
		{
			return self->IntersectBoundingSphere((BoundingSphere&)other); // TODO: Better cast?
		}
		else if (other.GetType() == TYPE_BOX)
		{
			return self->IntersectBoundingSphere((BoundingBox&)other); // TODO: Better cast?
		}
	}
	else if (mType == TYPE_BOX)
	{
		// TODO: 
	}

	return IntersectData(); // TODO: Should never reach this add error logs if we do.
}