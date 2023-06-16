#pragma once

#include "Animation.h"

namespace SAGE::Graphics
{
	class AnimationBuilder
	{
	public:
		AnimationBuilder& AddPositionKey(const Math::Vector3& position, float time);
		AnimationBuilder& AddRotationKey(const Math::Quaternion& rotation, float time);
		AnimationBuilder& AddScaleKey(const Math::Vector3& scale, float time);

		AnimationBuilder& SetLoop(bool isLooping);

		[[nodiscard]] Animation Get();
	private:
		Animation mWorkingCopy;
	};
}