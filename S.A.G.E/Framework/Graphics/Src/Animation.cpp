#include "Precompiled.h"
#include "Animation.h"

#include "Transform.h"

using namespace SAGE;
using namespace SAGE::Graphics;

namespace
{
	Math::Vector3 Interpolate(const Math::Vector3 v0, const Math::Vector3 v1, float t)
	{
		return Math::Lerp(v0, v1, t);
	}

	Math::Quaternion Interpolate(const Math::Quaternion q0, const Math::Quaternion q1, float t)
	{
		return Math::Slerp(q0, q1, t);
	}

	template<class T>
	T GetValue(const KeyFrames<T>& keys, float time)
	{
		if (time <= keys.front().time) {
			return keys.front().key;
		}
		else if (time >= keys.back().time) {
			return keys.back().key;
		}

		size_t index = 0;
		for (int i = 1; i < keys.size(); ++i)
		{
			if (time <= keys[i].time)
			{
				index = i - 1;
				break;
			}
		}

		const KeyFrame<T>& frontKey = keys[index];
		const KeyFrame<T>& endKey = keys[index + 1];

		const float t = (time - endKey.time) / (frontKey.time - endKey.time);
		float easeTime = t; // Easing::Linear

		switch (frontKey.easing)
		{
			case Easing::InSine: easeTime = 1.0f - cos((t * Math::Constants::Pi) * 0.5f); break;
			case Easing::OutSine: easeTime = sin((t * Math::Constants::Pi) * 0.5f); break;
			case Easing::InOutSine: easeTime = -(cos(t * Math::Constants::Pi) - 1.0f) * 0.5f; break;
			case Easing::InQuad: easeTime = t * t; break;
			case Easing::OutQuad: easeTime = 1.0f - (1.0f - t) * (1.0f - t); break;
			case Easing::InOutQuad: easeTime = t < 0.5f ? 2.0 * t * t : 1.0f - pow(-2.0f * t + 2.0f, 2) * 0.5f; break;
		}

		return Interpolate(endKey.key, frontKey.key, easeTime);
	}
}

Math::Vector3 Animation::GetPosition(float time) const
{
	return GetValue<Math::Vector3>(mPositionKeys, time);
}

Math::Quaternion Animation::GetRotation(float time) const
{
	return GetValue<Math::Quaternion>(mRotationKeys, time);
}

Math::Vector3 Animation::GetScale(float time) const
{
	return GetValue<Math::Vector3>(mScaleKeys, time);
}

Math::Matrix4 Animation::GetTransform(float time) const
{
	const auto& scale = GetScale(time);
	const auto& rotation = GetRotation(time);
	const auto& position = GetPosition(time);

	return Transform{ position, rotation, scale }.GetMatrix4();
}

float Animation::GetDuration() const
{
	return Math::Max(mPositionKeys.back().time, Math::Max(mRotationKeys.back().time, mScaleKeys.back().time));
}