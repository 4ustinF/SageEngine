#pragma once

#include "Common.h"

namespace SAGE::Graphics
{
	struct Transform
	{
		Transform() = default;
		Transform(const Math::Vector3& inPosition, const Math::Quaternion& inRotation, const Math::Vector3& inScale)
			: position(inPosition), rotation(inRotation), scale(inScale)
		{
		}

		Math::Vector3 position = Math::Vector3::Zero;
		Math::Quaternion rotation = Math::Quaternion::Identity;
		Math::Vector3 scale = Math::Vector3::One;

		[[nodiscard]] Math::Matrix4 GetMatrix4() const
		{
			return
			{
				Math::Matrix4::Scaling(scale) *
				Math::Matrix4::RotationQuaternion(rotation) *
				Math::Matrix4::Translation(position)
			};
		}
	};
}