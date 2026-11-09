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
		//return; // TODO:
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

bool Animator::PlayAnimation(size_t clipIndex, bool looping)
{
	ASSERT(clipIndex < mModel->animationSet.size(), "Animator -- Invalid animation clip index.");
	if (mClipIndex != clipIndex || IsFinished())
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
	if (clipIndex >= static_cast<size_t>(0))
	{
		return false;
	}

	return PlayAnimation(clipIndex, looping);
}

bool Animator::PlayAnimationInReverse(const std::string& clipName, bool looping)
{
	// TODO: Should we check if (mClipIndex != clipIndex || IsFinished()) to prevent a unnecessary lookup?
	const size_t clipIndex = GetClipIndexByName(clipName);
	if (clipIndex >= static_cast<size_t>(0))
	{
		return false;
	}

	return PlayAnimationInReverse(clipIndex, looping);
}

void Animator::Update(float deltaTime)
{
	if (!mIsAnimating) // Queue tick instead?
	{
		return;
	}

	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	mAnimationTick += clip.ticksPerSecond * deltaTime;
	if (mAnimationTick > clip.tickDuration)
	{
		if (mLooping) {
			while (mAnimationTick > clip.tickDuration)
			{
				mAnimationTick -= clip.tickDuration;
			}
		}
		else {
			mAnimationTick = clip.tickDuration;
			mIsAnimating = false;
			mOnAnimationFinished.Broadcast(mClipIndex);
		}
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

// -------------------- AnimationBlender --------------------

Animator* AnimationBlender::AddAnimator()
{
	return &mEntries.emplace_back(std::make_unique<Entry>())->animator;
}

void AnimationBlender::SetBlendWeight(size_t index, float weight)
{
	mEntries[index]->weight = weight;
}

void AnimationBlender::Update(float deltaTime)
{
	for (auto& entry : mEntries) {
		entry->animator.Update(deltaTime);
	}
}

Matrix4 AnimationBlender::GetTransform(const Bone* bone) const
{
	//Vector3 position = Vector3::Zero;
	//Quaternion rotation = Quaternion::Identity;
	//Vector3 scale = Vector3::One;

	//for (auto& entry : mEntries) 
	//{
	//	position = Lerp(position, entry->animator.GetPosition(bone), entry->weight);
	//	rotation = Slerp(rotation, entry->animator.GetRotation(bone), entry->weight);
	//	scale = Lerp(scale, entry->animator.GetScale(bone), entry->weight);
	//}

	//position = Lerp(
	//	mEntries[0]->animator.GetPosition(bone),
	//	mEntries[1]->animator.GetPosition(bone),
	//	mEntries[1]->weight );

	//rotation = Slerp(
	//	mEntries[0]->animator.GetRotation(bone),
	//	mEntries[1]->animator.GetRotation(bone),
	//	mEntries[1]->weight);

	//scale = Lerp(
	//	mEntries[0]->animator.GetScale(bone),
	//	mEntries[1]->animator.GetScale(bone),
	//	mEntries[1]->weight);

	//return Transform{ position, rotation, scale }.GetMatrix4();

	Matrix4 parentTransform1 = bone->toParentTransform;
	const AnimationClip& clip1 = mEntries[0]->animator.GetModel()->animationSet[mEntries[0]->animator.GetClipIndex()];
	if (clip1.HasTransform(bone->index)) {
		parentTransform1 = clip1.boneAnimations[bone->index]->GetTransform(mEntries[0]->animator.GetAnimationTick());
	}

	for (int i = 1; i < mEntries.size(); ++i)
	{
		if (mEntries[i]->weight <= 0.0f) {
			continue;
		}

		Matrix4 parentTransform2 = bone->toParentTransform;
		const AnimationClip& clip1 = mEntries[i]->animator.GetModel()->animationSet[mEntries[i]->animator.GetClipIndex()];
		if (clip1.HasTransform(bone->index)) {
			parentTransform2 = clip1.boneAnimations[bone->index]->GetTransform(mEntries[i]->animator.GetAnimationTick());
		}

		parentTransform1 = Lerp(parentTransform1, parentTransform2, mEntries[i]->weight);
	}

	return parentTransform1;
}