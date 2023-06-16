#include"Precompiled.h"
#include "AnimationBuilder.h"

using namespace SAGE;
using namespace SAGE::Graphics;

namespace
{
	template<class T, class K>
	void AddKey(T& keys, const K& info, float time)
	{
		ASSERT(keys.empty() || keys.back().time < time, "AnimationBuilder --- cannot add keyframe back in time.");
		keys.emplace_back(info, time);
	}
}

AnimationBuilder& AnimationBuilder::AddPositionKey(const Math::Vector3& position, float time)
{
	AddKey(mWorkingCopy.mPositionKeys, position, time);
	return *this;
}

AnimationBuilder& AnimationBuilder::AddRotationKey(const Math::Quaternion& rotation, float time)
{
	AddKey(mWorkingCopy.mRotationKeys, rotation, time);
	return *this;
}

AnimationBuilder& AnimationBuilder::AddScaleKey(const Math::Vector3& scale, float time)
{
	AddKey(mWorkingCopy.mScaleKeys, scale, time);
	return *this;
}

AnimationBuilder& AnimationBuilder::SetLoop(bool isLooping)
{
	mWorkingCopy.mLooping = isLooping;
	return *this;
}

Animation AnimationBuilder::Get()
{
	ASSERT(!mWorkingCopy.mPositionKeys.empty(), "AnimationBuilder --- Animation has no positiopn keys.");
	ASSERT(!mWorkingCopy.mRotationKeys.empty(), "AnimationBuilder --- Animation has no rotation keys.");
	ASSERT(!mWorkingCopy.mScaleKeys.empty(), "AnimationBuilder --- Animation has no scale keys.");
	return std::move(mWorkingCopy);
}