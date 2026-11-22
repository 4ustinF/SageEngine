#include "Precompiled.h"
#include "Animator.h"

#include "Model.h"
#include "Transform.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

void Animator::Initialize(const Model* model)
{
	if (mModel == model)
	{
		return;
	}

	mModel = model;
	mClipNameToIndexMap.clear();

	const std::vector<AnimationClip>& animationClips = mModel->animationSet;
	const size_t animationSetCount = animationClips.size();
	for (size_t i = 0; i < animationSetCount; ++i)
	{
		mClipNameToIndexMap.insert_or_assign(animationClips[i].name, i);
	}

	if (mClipNameToIndexMap.size() != animationSetCount)
	{
		mClipNameToIndexMap.clear(); // Clear the map if there are duplicate names to avoid incorrect lookups.
	}
}

bool Animator::StopAnimation()
{
	if (!mIsAnimating)
	{
		return false;
	}

	mIsAnimating = false;
	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	mAnimationTick = clip.tickDuration;
	// TODO: Do we broadcast any events here?
	return true;
}

bool Animator::PlayAnimation(size_t clipIndex, bool looping, bool force)
{
	ASSERT(clipIndex < mModel->animationSet.size(), "Animator -- Invalid animation clip index.");
	if (mClipIndex != clipIndex || IsFinished() || force) // mIsAnimating
	{
		mClipIndex = clipIndex;
		mAnimationTick = 0.0f;
		mLooping = looping;
		mReverse = false;
		mIsAnimating = true;
		return true;
	}

	return false;
}

bool Animator::PlayAnimationInReverse(size_t clipIndex, bool looping)
{
	if (PlayAnimation(clipIndex, looping))
	{
		mReverse = true;
		return true;
	}

	return false;
}

bool Animator::PlayAnimation(const std::string& clipName, bool looping)
{
	// TODO: Should we check if (mClipIndex != clipIndex || IsFinished()) to prevent a unnecessary lookup?
	const size_t clipIndex = GetClipIndexByName(clipName);
	if (clipIndex < static_cast<size_t>(0))
	{
		return false;
	}

	return PlayAnimation(clipIndex, looping);
}

bool Animator::PlayAnimationInReverse(const std::string& clipName, bool looping)
{
	// TODO: Should we check if (mClipIndex != clipIndex || IsFinished()) to prevent a unnecessary lookup?
	const size_t clipIndex = GetClipIndexByName(clipName);
	if (clipIndex < static_cast<size_t>(0))
	{
		return false;
	}

	return PlayAnimationInReverse(clipIndex, looping);
}

void Animator::Update(float deltaTime)
{
	if (!mIsAnimating)
	{
		return;
	}

	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	const float prevTick = mAnimationTick;
	mAnimationTick += clip.ticksPerSecond * deltaTime;

	if (mAnimationTick > clip.tickDuration)
	{
		if (mLooping) {
			FireEvents(clip, prevTick, clip.tickDuration, false);
			mAnimationTick = std::fmod(mAnimationTick, clip.tickDuration);
			FireEvents(clip, 0.0f, mAnimationTick, false);
		}
		else {
			mAnimationTick = clip.tickDuration;
			FireEvents(clip, prevTick, clip.tickDuration, true); // include events on the last tick
			mIsAnimating = false;
			mOnAnimationFinished.Broadcast(mClipIndex);
		}
	}
	else
	{
		FireEvents(clip, prevTick, mAnimationTick, false);
	}
}

bool Animator::IsFinished() const
{
	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	return !mLooping && (mAnimationTick == clip.tickDuration);
}

Vector3 Animator::GetPosition(const Bone* bone) const
{
	Vector3 position = GetTranslation(bone->toParentTransform);

	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	if (clip.HasTransform(bone->index)) {
		position = clip.boneAnimations[bone->index]->GetPosition(GetAnimationTick());
	}
	return position;
}

Quaternion Animator::GetRotation(const Bone* bone) const
{
	Quaternion rotation = Quaternion::RotationMatrix(bone->toParentTransform);

	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	if (clip.HasTransform(bone->index)) {
		rotation = clip.boneAnimations[bone->index]->GetRotation(GetAnimationTick());
	}
	return rotation;
}

Vector3 Animator::GetScale(const Bone* bone) const
{
	Vector3 scale = Vector3::One;

	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	if (clip.HasTransform(bone->index)) {
		scale = clip.boneAnimations[bone->index]->GetScale(GetAnimationTick());
	}
	return scale;
}

Matrix4 Animator::GetTransform(const Bone* bone) const
{
	Matrix4 toParentTransform = bone->toParentTransform;

	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	if (clip.HasTransform(bone->index)) {
		toParentTransform = clip.boneAnimations[bone->index]->GetTransform(GetAnimationTick());
	}
	return toParentTransform;
}

float Animator::GetAnimationTick() const
{
	if (mReverse)
	{
		const AnimationClip& clip = mModel->animationSet[mClipIndex]; // TODO: Cache this to avoid repeated lookups.
		return clip.tickDuration - mAnimationTick;
	}

	return mAnimationTick;
}

float Animator::GetNormalizedTime() const
{
	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	return clip.tickDuration > 0.0f ? mAnimationTick / clip.tickDuration : 0.0f;
}

float Animator::GetElapsedSeconds() const
{
	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	return clip.ticksPerSecond > 0.0f ? mAnimationTick / clip.ticksPerSecond : 0.0f;
}

float Animator::GetDurationSeconds() const
{
	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	return clip.ticksPerSecond > 0.0f ? clip.tickDuration / clip.ticksPerSecond : 0.0f;
}

const std::string Animator::GetClipNameByIndex(size_t index) const
{
	if (index < 0 || !mModel || index >= mModel->animationSet.size())
	{
		return "";
	}

	return mModel->animationSet[index].name;
}

size_t Animator::GetClipIndexByName(const std::string& name) const
{
	if (mClipNameToIndexMap.size() > 0)
	{
		if (auto it = mClipNameToIndexMap.find(name); it != mClipNameToIndexMap.end()) 
		{
			return it->second;
		}
	}
	else
	{
		// Fallback in case the map implementation didn't go through.
		const size_t animationSetCount = mModel->animationSet.size();
		for (size_t i = 0; i < animationSetCount; ++i)
		{
			if (mModel->animationSet[i].name == name)
			{
				return i;
			}
		}
	}

	// Failed to find.
	return static_cast<size_t>(-1);
}

void Animator::FireEvents(const AnimationClip& clip, float fromTick, float toTick, bool inclusiveEnd)
{
	for (const AnimationEvent& event : clip.events)
	{
		// In reverse playback the event sits at the mirrored position.
		const float t = mReverse ? clip.tickDuration - event.tick : event.tick;

		const bool inRange = (t >= fromTick) && (inclusiveEnd ? t <= toTick : t < toTick);
		if (inRange)
		{
			mOnAnimationEvent.Broadcast(mClipIndex, event.name);
		}
	}
}

// -------------------- AnimationBlender --------------------

void AnimationBlender::Update(float deltaTime)
{
	for (auto& entry : mEntries) {
		entry->animator.Update(deltaTime);
	}
}

void AnimationBlender::SetBase(const Animator* base) 
{ 
	mBase = base; 
}

void AnimationBlender::SetBlendWeight(size_t index, float weight)
{
	mEntries[index]->weight = weight;
}

Animator* AnimationBlender::AddAnimator()
{
	return &mEntries.emplace_back(std::make_unique<Entry>())->animator;
}

Matrix4 AnimationBlender::GetTransform(const Bone* bone) const
{
	Matrix4 transform = mBase->GetTransform(bone);
	for (const auto& layer : mEntries)
	{
		if (!layer->animator.GetIsAnimating())
		{
			continue;
		}

		const float w = layer->weight;
		if (w <= 0.0f)
		{
			continue;
		}

		transform = Lerp(transform, layer->animator.GetTransform(bone), w);
	}

	return transform;
}