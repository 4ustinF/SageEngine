#include "Precompiled.h"
#include "AnimationUtil.h"

#include "SimpleDraw.h"

using namespace SAGE;
using namespace SAGE::Graphics;

namespace
{
	void ComputeAnimationTransform(Bone* bone, const AnimationClip& clip, float animationTime, std::vector<Math::Matrix4>& boneTransforms)
	{
		if (bone->parent) {
			Math::Matrix4 toParentTransform = bone->toParentTransform;
			if (clip.HasTransform(bone->index)) {
				toParentTransform = clip.boneAnimations[bone->index]->GetTransform(animationTime);
			}

			boneTransforms[bone->index] = toParentTransform * boneTransforms[bone->parentIndex];
		}

		for (auto child : bone->children) {
			ComputeAnimationTransform(child, clip, animationTime, boneTransforms);
		}
	}

	void ComputeTransform(Bone* bone, std::vector<Math::Matrix4>& boneTransforms, AnimationUtil::GetBoneTransform getBoneTransform)
	{
		if (bone->parent) {
			boneTransforms[bone->index] = getBoneTransform(bone) * boneTransforms[bone->parentIndex];
		}

		for (auto child : bone->children) {
			ComputeTransform(child, boneTransforms, getBoneTransform);
		}
	}
}

void AnimationUtil::ComputeBoneTransforms(const Skeleton& skeleton, std::vector<Math::Matrix4>& boneTransforms, GetBoneTransform getBoneTransform)
{
	boneTransforms.resize(skeleton.bones.size(), Math::Matrix4::Identity);
	ComputeTransform(skeleton.root, boneTransforms, getBoneTransform);
}

void AnimationUtil::ApplyBoneOffset(const Skeleton& skeleton, std::vector<Math::Matrix4>& boneTransforms)
{
	for (auto& bone : skeleton.bones) {
		boneTransforms[bone->index] = bone->offSetTransform * boneTransforms[bone->index];
	}
}

void AnimationUtil::DrawSkeleton(const Skeleton& skeleton, const std::vector<Math::Matrix4>& boneTransforms)
{
	for (auto& bone : skeleton.bones)
	{
		const Bone* parentBone = bone->parent;
		if (parentBone != nullptr)
		{
			const Math::Vector3& bonePosition = Math::GetTranslation(boneTransforms[bone->index]);
			const Math::Vector3& parentPosition = Math::GetTranslation(boneTransforms[parentBone->index]);

			SimpleDraw::AddLine(bonePosition, parentPosition, Colors::Green);
			SimpleDraw::AddSphere(bonePosition, 4, 2, 0.05f, Colors::Cyan);
		}
	}
}