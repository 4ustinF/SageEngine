#pragma once

#include "Common.h"

namespace SAGE::RBPhysics
{
	class IntersectData;

	class Collider
	{
	public:
		enum
		{
			TYPE_SPHERE,
			TYPE_AABB,

			TYPE_SIZE,
		};

		Collider(int type)
			: mType(type)
		{
		}

		IntersectData Intersect(const Collider& other) const;

		int GetType() const { return mType; }

	private:
		int mType;
	};
}