#include "Precompiled.h"
#include "Collider.h"

#include "BoundingBox.h"
#include "BoundingCapsule.h"
#include "BoundingSphere.h"
#include "IntersectData.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;

IntersectData Collider::Intersect(const Collider* other) const
{
	if (mType == TYPE_SPHERE)
	{
		//BoundingSphere* self = (BoundingSphere*)this; // TODO: Safer cast?

		//if (other->GetType() == ColliderType::TYPE_BOX)
		//{
		//	return self->IntersectBoundingSphere((BoundingBox&)other); // TODO: Better cast?
		//}
		//else if (other->GetType() == ColliderType::TYPE_CAPSULE)
		//{
		//	//return self->IntersectBoundingSphere((BoundingCapsule&)other); // TODO: Better cast?
		//}
		//else if (other->GetType() == ColliderType::TYPE_SPHERE)
		//{
		//	return self->IntersectBoundingSphere((BoundingSphere&)other); // TODO: Better cast?
		//}
	}
	else if (mType == ColliderType::TYPE_BOX)
	{
		// TODO: 
	}
	else if (mType == ColliderType::TYPE_CAPSULE)
	{
		BoundingCapsule* self = (BoundingCapsule*)this; // TODO: Safer cast?
		if (other->GetType() == ColliderType::TYPE_BOX)
		{
			return self->IntersectBoundingCapsule(dynamic_cast<const BoundingBox*>(other));
		}
		else if (other->GetType() == ColliderType::TYPE_CAPSULE)
		{
			return self->IntersectBoundingCapsule(dynamic_cast<const BoundingCapsule*>(other));
		}
	}

	return IntersectData(); // TODO: Should never reach this add error logs if we do.
}