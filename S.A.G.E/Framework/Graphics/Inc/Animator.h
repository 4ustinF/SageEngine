#pragma once

namespace SAGE::Graphics
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnimationFinished, size_t); // Clip Index

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

		bool PlayAnimation(size_t clipIndex, bool looping = true);
		bool PlayAnimationInReverse(size_t clipIndex, bool looping = true);
		bool PlayAnimation(const std::string& clipName, bool looping = true);
		bool PlayAnimationInReverse(const std::string& clipName, bool looping = true);
		void Update(float deltaTime);
		bool IsFinished() const;

		Math::Vector3 GetPosition(const Bone* bone) const;
		Math::Quaternion GetRotation(const Bone* bone) const;
		Math::Vector3 GetScale(const Bone* bone) const;

		Math::Matrix4 GetTransform(const Bone* bone) const override;

		const Model* GetModel() const { return mModel; }
		size_t GetClipIndex() const { return mClipIndex; }
		float GetAnimationTick() const { return mAnimationTick; }
		bool GetIsLooping() const { return mLooping; }

		const std::string GetClipNameByIndex(size_t index) const;
		size_t GetClipIndexByName(const std::string& name) const;

		FOnAnimationFinished& GetOnAnimationFinishedDelegate() { return mOnAnimationFinished; }

	private:
		const Model* mModel = nullptr;
		size_t mClipIndex = 0;
		float mAnimationTick = 0.0f;
		bool mLooping = true;
		bool mReverse = false;

		bool mIsAnimating = false;
		FOnAnimationFinished mOnAnimationFinished;

		std::unordered_map<std::string, size_t> mClipNameToIndexMap;
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