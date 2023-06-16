#include "Precompiled.h"
#include "Animator.h"

#include "Model.h"
#include "Transform.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void Animator::Initialize(const Model* model)
{
	mModel = model;
}

void Animator::PlayAnimation(size_t clipIndex, bool looping)
{
	ASSERT(clipIndex < mModel->animationSet.size(), "Animator -- Invalid animation clip index.");
	if (mClipIndex != clipIndex)
	{
		mClipIndex = clipIndex;
		mAnimationTick = 0.0f;
		mLooping = looping;
	}
}

void Animator::Update(float deltaTime)
{
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
		}
	}
}

bool Animator::IsFinished() const
{
	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	return !mLooping && (mAnimationTick == clip.tickDuration);
}

Math::Vector3 Animator::GetPosition(const Bone* bone) const
{
	Math::Vector3 position = Math::GetTranslation(bone->toParentTransform);

	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	if (clip.HasTransform(bone->index)) {
		position = clip.boneAnimations[bone->index]->GetPosition(mAnimationTick);
	}
	return position;
}

Math::Quaternion Animator::GetRotation(const Bone* bone) const
{
	Math::Quaternion rotation = Math::Quaternion::RotationMatrix(bone->toParentTransform);

	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	if (clip.HasTransform(bone->index)) {
		rotation = clip.boneAnimations[bone->index]->GetRotation(mAnimationTick);
	}
	return rotation;
}

Math::Vector3 Animator::GetScale(const Bone* bone) const
{
	Math::Vector3 scale = Math::Vector3::One;

	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	if (clip.HasTransform(bone->index)) {
		scale = clip.boneAnimations[bone->index]->GetScale(mAnimationTick);
	}
	return scale;
}

Math::Matrix4 Animator::GetTransform(const Bone* bone) const
{
	Math::Matrix4 toParentTransform = bone->toParentTransform;

	const AnimationClip& clip = mModel->animationSet[mClipIndex];
	if (clip.HasTransform(bone->index)) {
		toParentTransform = clip.boneAnimations[bone->index]->GetTransform(mAnimationTick);
	}
	return toParentTransform;
}

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

Math::Matrix4 AnimationBlender::GetTransform(const Bone* bone) const
{
	//Math::Vector3 position = Math::Vector3::Zero;
	//Math::Quaternion rotation = Math::Quaternion::Identity;
	//Math::Vector3 scale = Math::Vector3::One;

	//for (auto& entry : mEntries) 
	//{
	//	position = Math::Lerp(position, entry->animator.GetPosition(bone), entry->weight);
	//	rotation = Math::Slerp(rotation, entry->animator.GetRotation(bone), entry->weight);
	//	scale = Math::Lerp(scale, entry->animator.GetScale(bone), entry->weight);
	//}

	//position = Math::Lerp(
	//	mEntries[0]->animator.GetPosition(bone),
	//	mEntries[1]->animator.GetPosition(bone),
	//	mEntries[1]->weight );

	//rotation = Math::Slerp(
	//	mEntries[0]->animator.GetRotation(bone),
	//	mEntries[1]->animator.GetRotation(bone),
	//	mEntries[1]->weight);

	//scale = Math::Lerp(
	//	mEntries[0]->animator.GetScale(bone),
	//	mEntries[1]->animator.GetScale(bone),
	//	mEntries[1]->weight);

	//return Transform{ position, rotation, scale }.GetMatrix4();

	Math::Matrix4 parentTransform1 = bone->toParentTransform;
	const AnimationClip& clip1 = mEntries[0]->animator.mModel->animationSet[mEntries[0]->animator.mClipIndex];
	if (clip1.HasTransform(bone->index)) {
		parentTransform1 = clip1.boneAnimations[bone->index]->GetTransform(mEntries[0]->animator.mAnimationTick);
	}

	for (int i = 1; i < mEntries.size(); ++i)
	{
		if (mEntries[i]->weight <= 0.0f) {
			continue;
		}

		Math::Matrix4 parentTransform2 = bone->toParentTransform;
		const AnimationClip& clip1 = mEntries[i]->animator.mModel->animationSet[mEntries[i]->animator.mClipIndex];
		if (clip1.HasTransform(bone->index)) {
			parentTransform2 = clip1.boneAnimations[bone->index]->GetTransform(mEntries[i]->animator.mAnimationTick);
		}

		parentTransform1 = Math::Lerp(parentTransform1, parentTransform2, mEntries[i]->weight);
	}

	return parentTransform1;
}