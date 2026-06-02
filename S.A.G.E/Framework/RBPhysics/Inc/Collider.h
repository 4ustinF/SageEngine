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
			TYPE_BOX,
			TYPE_SPHERE,

			TYPE_SIZE,
		};

		Collider(int type)
			: mType(type)
		{
		}

		virtual void DebugDraw(SAGE::Math::Quaternion orientation, bool fillDebugShapes) {}

		IntersectData Intersect(const Collider& other) const;
		virtual void Transform(const Math::Vector3& translation) {}
		Math::Vector3 GetCenter() const { return mCenter; }
		const SAGE::Math::Quaternion& GetOrientation() const { return mOrientation; }

		int GetType() const { return mType; }
		
		// Reference Counter
		int GetReferenceCount() { return mRefCount; }
		void AddReference() { mRefCount++; }
		bool RemoveReference() { return --mRefCount == 0; }

		void SetDebugColor(SAGE::Graphics::Color debugColor) { mDebugColor = debugColor; }

	protected:
		SAGE::Graphics::Color mDebugColor = SAGE::Graphics::Colors::Red;
		SAGE::Math::Vector3 mCenter = SAGE::Math::Vector3::Zero;
		SAGE::Math::Quaternion mOrientation = SAGE::Math::Quaternion::Identity;

	private:
		int mType;
		int mRefCount = 0;

	};
}