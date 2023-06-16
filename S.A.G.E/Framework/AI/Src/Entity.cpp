#include "Precompiled.h"
#include "Entity.h"

#include "AIWorld.h"

using namespace AI;
using namespace SAGE;
using namespace SAGE::Math;

Entity::Entity(AIWorld& world, uint32_t typeID)
	: world(world) 
	, mUniqueID(static_cast<uint64_t>(typeID) << 32 | world.GetnextID())
{
	world.RegisterEntity(this);
}

Entity::~Entity()
{
	world.UnRegisterEntity(this);
}

Matrix3 Entity::GetWorldTransform() const
{
	const auto h = heading;
	const auto s = PerpendicularRH(h);
	const auto d = position;

	return{
		s.x, s.y, 0.0f,
		h.x, h.y, 0.0f,
		d.x, d.y, 1.0f
	};
}