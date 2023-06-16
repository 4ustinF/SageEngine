#pragma once

namespace SAGE::Graphics
{
	struct Bone;
	struct Model;

	struct IAnimator
	{
		virtual Math::Matrix4 GetTransform(const Bone* bone) const = 0;
	};

	class Animator final : public IAnimator
	{
	public:
		void Initialize(const Model* model);

		void PlayAnimation(size_t clipIndex, bool looping = true);
		void Update(float deltaTime);
		bool IsFinished() const;

		Math::Vector3 GetPosition(const Bone* bone) const;
		Math::Quaternion GetRotation(const Bone* bone) const;
		Math::Vector3 GetScale(const Bone* bone) const;

		Math::Matrix4 GetTransform(const Bone* bone) const override;

	//private:
		const Model* mModel = nullptr;
		size_t mClipIndex = 0;
		float mAnimationTick = 0.0f;
		bool mLooping = false;
	};

	class AnimationBlender final : public IAnimator
	{
	public:
		Animator* AddAnimator();

		void SetBlendWeight(size_t index, float weight);

		void Update(float deltaTime);
		Math::Matrix4 GetTransform(const Bone* bone) const override;

	private:
		struct Entry
		{
			Animator animator;
			float weight = 0.0f;
		};
		std::vector<std::unique_ptr<Entry>> mEntries;
	};
}