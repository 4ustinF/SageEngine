#include "Precompiled.h"
#include "Animator.h"

#include "Model.h"
#include "Transform.h"

#include <limits>

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

namespace
{
	constexpr size_t kInvalidClip = static_cast<size_t>(-1);

	float MoveToward(float current, float target, float maxDelta)
	{
		if (std::abs(target - current) <= maxDelta)
		{
			return target;
		}
		return current + (target > current ? maxDelta : -maxDelta);
	}
}

void Animator::Initialize(const Model* model)
{
	if (model == nullptr)
	{
		mModel = nullptr;
		mClipNameToIndexMap.clear();
		return;
	}

	// Same model and no clips added since last time: nothing to do.
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

bool Animator::PlayAnimation(size_t clipIndex, bool looping, bool force)
{
	ASSERT(clipIndex < mModel->animationSet.size(), "Animator -- Invalid animation clip index.");
	if (mClipIndex != clipIndex || IsFinished() || force)
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

bool Animator::PlayAnimationInReverse(size_t clipIndex, bool looping, bool force)
{
	if (PlayAnimation(clipIndex, looping, force))
	{
		mReverse = true;
		return true;
	}

	return false;
}

bool Animator::PlayAnimation(const std::string& clipName, bool looping, bool force)
{
	const size_t clipIndex = GetClipIndexByName(clipName);
	if (clipIndex == kInvalidClip) // was `< 0` on an unsigned type, which is never true
	{
		return false;
	}

	return PlayAnimation(clipIndex, looping, force);
}

bool Animator::PlayAnimationInReverse(const std::string& clipName, bool looping)
{
	const size_t clipIndex = GetClipIndexByName(clipName);
	if (clipIndex == kInvalidClip)
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
		const AnimationClip& clip = mModel->animationSet[mClipIndex];
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
	if (!mModel || index >= mModel->animationSet.size()) // `index < 0` removed, unsigned can't be negative
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
	else if (mModel)
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
	return kInvalidClip;
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

float AnimationBlender::Layer::GetBoneWeight(const Bone* bone) const
{
	if (!useBoneMask)
	{
		return weight;
	}

	const size_t index = static_cast<size_t>(bone->index);
	return index < boneMask.size() ? weight * boneMask[index] : 0.0f;
}

Animator* AnimationBlender::AddLayer()
{
	return &mLayers.emplace_back(std::make_unique<Layer>())->animator;
}

void AnimationBlender::SetLayerWeight(size_t index, float weight)
{
	Layer& layer = *mLayers[index];
	layer.weight = weight;
	layer.targetWeight = weight;
	layer.autoFadeOut = false;
}

void AnimationBlender::BlendLayerTo(size_t index, float target, float blendSpeed)
{
	Layer& layer = *mLayers[index];
	layer.targetWeight = target;
	layer.blendSpeed = blendSpeed;
	layer.autoFadeOut = false;
}

bool AnimationBlender::PlayLayerOneShot(size_t index, const std::string& clipName, float blendInSpeed, float fadeOutSeconds)
{
	const size_t clipIndex = mLayers[index]->animator.GetClipIndexByName(clipName);
	if (clipIndex == kInvalidClip)
	{
		return false;
	}

	return PlayLayerOneShot(index, clipIndex, blendInSpeed, fadeOutSeconds);
}

bool AnimationBlender::PlayLayerOneShot(size_t index, size_t clipIndex, float blendInSpeed, float fadeOutSeconds)
{
	Layer& layer = *mLayers[index];
	if (!layer.animator.PlayAnimation(clipIndex, false, true)) // force restart so rapid fire works
	{
		return false;
	}

	layer.targetWeight = 1.0f;
	layer.blendSpeed = blendInSpeed;
	layer.fadeOutSeconds = fadeOutSeconds;
	layer.autoFadeOut = true;
	return true;
}

void AnimationBlender::SetLayerBoneMask(size_t index, const Bone* rootBone, bool includeRoot)
{
	Layer& layer = *mLayers[index];
	layer.boneMask.clear();
	layer.useBoneMask = true; // a mask with nothing set means "affect no bones", not "affect all"

	if (rootBone == nullptr)
	{
		return;
	}

	// Mark the bone (optionally) and everything below it.
	auto mark = [&layer](auto&& self, const Bone* bone, bool include) -> void
		{
			if (include)
			{
				const size_t boneIndex = static_cast<size_t>(bone->index);
				if (layer.boneMask.size() <= boneIndex)
				{
					layer.boneMask.resize(boneIndex + 1, 0.0f);
				}
				layer.boneMask[boneIndex] = 1.0f;
			}

			for (const Bone* child : bone->children)
			{
				self(self, child, true);
			}
		};
	mark(mark, rootBone, includeRoot);
}

void AnimationBlender::ClearLayerBoneMask(size_t index)
{
	Layer& layer = *mLayers[index];
	layer.useBoneMask = false;
	layer.boneMask.clear();
}

void AnimationBlender::Update(float deltaTime)
{
	for (auto& layerPtr : mLayers)
	{
		Layer& layer = *layerPtr;
		Animator& animator = layer.animator;
		animator.Update(deltaTime);

		if (layer.autoFadeOut)
		{
			const float remaining = animator.GetDurationSeconds() - animator.GetElapsedSeconds();
			if (!animator.IsPlaying() || remaining <= layer.fadeOutSeconds)
			{
				layer.targetWeight = 0.0f;
				layer.blendSpeed = 1.0f / std::max(layer.fadeOutSeconds, 0.001f);
				layer.autoFadeOut = false;
			}
		}

		layer.weight = MoveToward(layer.weight, layer.targetWeight, layer.blendSpeed * deltaTime);
	}
}

Matrix4 AnimationBlender::GetTransform(const Bone* bone) const
{
	if (mBase == nullptr)
	{
		return bone->toParentTransform;
	}

	Vector3 position = mBase->GetPosition(bone);
	Quaternion rotation = mBase->GetRotation(bone);
	Vector3 scale = mBase->GetScale(bone);

	for (const auto& layer : mLayers)
	{
		const float w = layer->GetBoneWeight(bone);
		if (w <= 0.0f)
		{
			continue; // idle / masked-out layers cost nothing
		}

		const Animator& animator = layer->animator;
		position = Lerp(position, animator.GetPosition(bone), w);
		rotation = Slerp(rotation, animator.GetRotation(bone), w); // make sure Slerp takes the shortest path
		scale = Lerp(scale, animator.GetScale(bone), w);
	}

	return Transform{ position, rotation, scale }.GetMatrix4();
}


// -------------------- AnimationCrossFader --------------------

void AnimationCrossFader::Initialize(const Model* model)
{
	for (Slot& slot : mSlots)
	{
		slot.animator.Initialize(model);
		if (!mEventsBound)
		{
			slot.eventHandle = slot.animator.GetOnAnimationEventDelegate().AddRaw(this, &AnimationCrossFader::ForwardEvent);
		}
	}
	mEventsBound = true;
}

size_t AnimationCrossFader::GetCurrentClipIndex() const
{
	return mCurrent != kNone ? mSlots[mCurrent].animator.GetClipIndex() : kInvalidClip;
}

size_t AnimationCrossFader::AcquireSlot() const
{
	for (size_t i = 0; i < kMaxSlots; ++i)
	{
		if (!mSlots[i].active)
		{
			return i;
		}
	}

	// Every slot is busy (extreme spam). Steal the quietest one that isn't the current clip.
	size_t quietest = kNone;
	float lowestWeight = std::numeric_limits<float>::max();
	for (size_t i = 0; i < kMaxSlots; ++i)
	{
		if (i != mCurrent && mSlots[i].weight < lowestWeight)
		{
			lowestWeight = mSlots[i].weight;
			quietest = i;
		}
	}
	return quietest;
}

bool AnimationCrossFader::Play(size_t clipIndex, bool looping, float blendSeconds, bool reverse)
{
	const Model* model = mSlots[0].animator.GetModel();
	ASSERT(model != nullptr, "AnimationCrossFader -- Play called before the model was loaded/initialized.");
	ASSERT(model == nullptr || clipIndex < model->animationSet.size(), "AnimationCrossFader -- Invalid animation clip index.");
	if (model == nullptr || clipIndex >= model->animationSet.size())
	{
		return false; // release-build safety net; the asserts above are the real signal
	}

	const size_t index = AcquireSlot();
	Slot& slot = mSlots[index];

	const bool instant = (mCurrent == kNone) || (blendSeconds <= 0.0f);

	// Always force so a clip can restart / fade into itself.
	if (reverse)
	{
		slot.animator.PlayAnimationInReverse(clipIndex, looping, true);
	}
	else
	{
		slot.animator.PlayAnimation(clipIndex, looping, true);
	}

	slot.active = true;
	slot.notifyOnFinish = !looping;
	slot.targetWeight = 1.0f;
	slot.weight = instant ? 1.0f : 0.0f;
	slot.fadeSpeed = instant ? 0.0f : 1.0f / blendSeconds;

	// Everything that was showing starts fading out (or is dropped immediately for an instant switch).
	for (size_t i = 0; i < kMaxSlots; ++i)
	{
		Slot& other = mSlots[i];
		if (i == index || !other.active)
		{
			continue;
		}

		other.notifyOnFinish = false; // superseded: its ending no longer matters
		if (instant)
		{
			other.active = false;
			other.weight = 0.0f;
		}
		else
		{
			other.targetWeight = 0.0f;
			other.fadeSpeed = 1.0f / blendSeconds;
		}
	}

	mCurrent = index;
	RefreshSoloCache();
	return true;
}

bool AnimationCrossFader::Play(const std::string& clipName, bool looping, float blendSeconds, bool reverse)
{
	const size_t clipIndex = GetClipIndexByName(clipName);
	if (clipIndex == kInvalidClip)
	{
		return false;
	}

	return Play(clipIndex, looping, blendSeconds, reverse);
}

void AnimationCrossFader::Update(float deltaTime)
{
	bool currentFinished = false;
	size_t finishedClip = kInvalidClip;

	for (size_t i = 0; i < kMaxSlots; ++i)
	{
		Slot& slot = mSlots[i];
		if (!slot.active)
		{
			continue;
		}

		mUpdatingSlot = i;
		slot.animator.Update(deltaTime);

		if (i == mCurrent && slot.notifyOnFinish && !slot.animator.IsPlaying())
		{
			slot.notifyOnFinish = false;
			currentFinished = true;
			finishedClip = slot.animator.GetClipIndex();
		}

		slot.weight = MoveToward(slot.weight, slot.targetWeight, slot.fadeSpeed * deltaTime);
		if (i != mCurrent && slot.weight <= 0.0f)
		{
			slot.active = false; // fully faded out: stop ticking it and free the slot
		}
	}
	mUpdatingSlot = kNone;

	RefreshSoloCache();

	// Broadcast last so a handler that calls Play() sees a consistent state and the freshly started slot doesn't get an extra tick this frame.
	if (currentFinished)
	{
		mOnCurrentFinished.Broadcast(finishedClip);
	}
}

void AnimationCrossFader::RefreshSoloCache()
{
	size_t contributing = 0;
	size_t solo = kNone;
	for (size_t i = 0; i < kMaxSlots; ++i)
	{
		if (mSlots[i].active && mSlots[i].weight > 0.0f)
		{
			++contributing;
			solo = i;
		}
	}
	mSolo = (contributing == 1) ? solo : kNone;
}

Matrix4 AnimationCrossFader::GetTransform(const Bone* bone) const
{
	// Common case: nothing is fading, so no decompose/blend is needed.
	if (mSolo != kNone)
	{
		return mSlots[mSolo].animator.GetTransform(bone);
	}

	Vector3 position = Vector3::Zero;
	Quaternion rotation = Quaternion::Identity;
	Vector3 scale = Vector3::One;
	float accumulated = 0.0f;
	bool hasPose = false;

	// Running weighted average: after each step the pose equals the weighted mean of the slots so far,
	// so the weights don't need to sum to 1.
	for (const Slot& slot : mSlots)
	{
		if (!slot.active || slot.weight <= 0.0f)
		{
			continue;
		}

		const Animator& animator = slot.animator;
		if (!hasPose)
		{
			position = animator.GetPosition(bone);
			rotation = animator.GetRotation(bone);
			scale = animator.GetScale(bone);
			accumulated = slot.weight;
			hasPose = true;
			continue;
		}

		accumulated += slot.weight;
		const float t = slot.weight / accumulated;
		position = Lerp(position, animator.GetPosition(bone), t);
		rotation = Slerp(rotation, animator.GetRotation(bone), t); // make sure Slerp takes the shortest path
		scale = Lerp(scale, animator.GetScale(bone), t);
	}

	if (!hasPose)
	{
		return bone->toParentTransform;
	}

	return Transform{ position, rotation, scale }.GetMatrix4();
}

AnimationCrossFader::SlotDebugInfo AnimationCrossFader::GetSlotDebugInfo(size_t index) const
{
	SlotDebugInfo info;
	const Slot& slot = mSlots[index];

	info.isCurrent = (index == mCurrent);
	info.weight = slot.weight;
	info.targetWeight = slot.targetWeight;
	info.active = slot.active && slot.animator.GetModel() != nullptr;
	if (info.active)
	{
		info.clipIndex = slot.animator.GetClipIndex();
		info.elapsedSeconds = slot.animator.GetElapsedSeconds();
		info.durationSeconds = slot.animator.GetDurationSeconds();
		info.looping = slot.animator.GetIsLooping();
		info.playing = slot.animator.IsPlaying();
	}
	return info;
}

void AnimationCrossFader::ForwardEvent(size_t clipIndex, const std::string& eventName)
{
	if (mUpdatingSlot == mCurrent) // a fading-out clip must not fire sounds/effects
	{
		mOnAnimationEvent.Broadcast(clipIndex, eventName);
	}
}