#pragma once

#include "Common.h"

namespace SAGE::RBPhysics
{
	class IntersectData;

	class Collider
	{
	public:
		enum ColliderType
		{
			TYPE_BOX,
			TYPE_CAPSULE,
			TYPE_SPHERE,

			TYPE_SIZE,
		};

		Collider(ColliderType type)
			: mType(type)
		{
		}

		virtual void DebugDraw(SAGE::Math::Quaternion orientation, bool fillDebugShapes) {}

		IntersectData Intersect(const Collider& other) const;
		virtual void Transform(const Math::Vector3& translation) {}
		Math::Vector3 GetCenter() const { return mCenter; }
		const SAGE::Math::Quaternion& GetOrientation() const { return mOrientation; } // TODO: Does collider need to hold this info?

		Math::Matrix4 GetMatrix4() const
		{
			return
			{
				Math::Matrix4::Scaling(Math::Vector3::One) *
				Math::Matrix4::RotationQuaternion(mOrientation) *
				Math::Matrix4::Translation(mCenter)
			};
		}

		Math::Matrix4 GetRotationMatrix4() const
		{
			return
			{
				Math::Matrix4::RotationQuaternion(mOrientation)
			};
		}

		ColliderType GetType() const { return mType; }
		
		// Reference Counter
		int GetReferenceCount() { return mRefCount; }
		void AddReference() { mRefCount++; }
		bool RemoveReference() { return --mRefCount == 0; }

		void SetDebugColor(SAGE::Graphics::Color debugColor) { mDebugColor = debugColor; }

	protected:
		SAGE::Graphics::Color mDebugColor = SAGE::Graphics::Colors::Red;
		SAGE::Math::Vector3 mCenter = SAGE::Math::Vector3::Zero;
		SAGE::Math::Quaternion mOrientation = SAGE::Math::Quaternion::Identity; // TODO: When we add orientation we need a way for box collider to know the changes and update is axis alligned.

	private:
		ColliderType mType;
		int mRefCount = 0;

	};
}