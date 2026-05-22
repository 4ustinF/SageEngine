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
		virtual void Transform(const Math::Vector3& translation) {}
		virtual Math::Vector3 GetCenter() const { return Math::Vector3::Zero; }

		int GetType() const { return mType; }
		
		// Reference Counter
		int GetReferenceCount() { return mRefCount; }
		void AddReference() { mRefCount++; }
		bool RemoveReference() { return --mRefCount == 0; }

	private:
		int mType;
		int mRefCount = 0;
	};
}