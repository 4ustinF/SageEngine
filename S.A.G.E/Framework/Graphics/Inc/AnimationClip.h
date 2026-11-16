#pragma once

#include "Common.h"
#include "Animation.h"

namespace SAGE::Graphics
{
	struct AnimationEvent
	{
		float tick = 0.0f;   // same units as tickDuration
		std::string name;    
	};

	class AnimationClip
	{
	public:
		bool HasTransform(uint32_t boneIndex) const
		{
			const auto& boneAnim = boneAnimations[boneIndex];
			return (boneAnim != nullptr);
		}

		std::string name;
		float tickDuration = 0.0f;
		float ticksPerSecond = 0.0f;

		// This will have the same size as bones in skeleton, so we can use bone->index to index this vector.
		// Though not all bones will animate so we will need to have some null slots to keep this format
		std::vector<std::unique_ptr<Animation>> boneAnimations;

		std::vector<AnimationEvent> events;
	};
}