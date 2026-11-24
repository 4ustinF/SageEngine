#pragma once

namespace SAGE::Graphics
{
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnimationFinished, size_t); // Clip Index
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAnimationEvent, size_t, const std::string&); // clipIndex, eventName

	class AnimationClip;
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

		bool PlayAnimation(size_t clipIndex, bool looping = true, bool force = false);
		bool PlayAnimationInReverse(size_t clipIndex, bool looping = true, bool force = false);
		bool PlayAnimation(const std::string& clipName, bool looping = true, bool force = false);
		bool PlayAnimationInReverse(const std::string& clipName, bool looping = true);
		void Update(float deltaTime);
		bool IsFinished() const;
		bool IsPlaying() const { return mIsAnimating; } // false before the first Play and after a one-shot ends

		Math::Vector3 GetPosition(const Bone* bone) const;
		Math::Quaternion GetRotation(const Bone* bone) const;
		Math::Vector3 GetScale(const Bone* bone) const;
		Math::Matrix4 GetTransform(const Bone* bone) const override;

		const Model* GetModel() const { return mModel; }
		size_t GetClipIndex() const { return mClipIndex; }
		float GetAnimationTick() const;
		bool GetIsLooping() const { return mLooping; }
		bool GetIsReverse() const { return mReverse; }

		float GetNormalizedTime() const;   // 0..1
		float GetElapsedSeconds() const;   // seconds into the clip
		float GetDurationSeconds() const;

		const std::string GetClipNameByIndex(size_t index) const;
		size_t GetClipIndexByName(const std::string& name) const;

		FOnAnimationEvent& GetOnAnimationEventDelegate() { return mOnAnimationEvent; }
		FOnAnimationFinished& GetOnAnimationFinishedDelegate() { return mOnAnimationFinished; }

		void SetIsLooping(bool isLooping) { mLooping = isLooping; }

	private:
		const Model* mModel = nullptr;
		size_t mClipIndex = 0;
		float mAnimationTick = 0.0f;
		bool mLooping = true;
		bool mReverse = false;

		bool mIsAnimating = false;
		FOnAnimationFinished mOnAnimationFinished;

		// Events
		void FireEvents(const AnimationClip& clip, float fromTick, float toTick, bool inclusiveEnd);
		FOnAnimationEvent mOnAnimationEvent;

		std::unordered_map<std::string, size_t> mClipNameToIndexMap;
	};

	// Layers extra animators on top of a base animator (owned by someone else, e.g. AnimatorComponent).
	// Each layer has a weight (0 = no effect, 1 = fully overrides what's below) and an optional per-bone mask.
	class AnimationBlender final : public IAnimator
	{
	public:
		void SetBase(const Animator* base) { mBase = base; }

		Animator* AddLayer();
		Animator* GetLayer(size_t index) { return &mLayers[index]->animator; }
		size_t GetLayerCount() const { return mLayers.size(); }

		// Immediately sets the weight (no smoothing).
		void SetLayerWeight(size_t index, float weight);
		float GetLayerWeight(size_t index) const { return mLayers[index]->weight; }

		// Smoothly moves the weight toward target. blendSpeed is weight-per-second (15 ~= 0.07s full blend).
		void BlendLayerTo(size_t index, float target, float blendSpeed = 15.0f);

		// Restarts the clip on the layer, blends in, then automatically blends out near the end of the clip.
		bool PlayLayerOneShot(size_t index, const std::string& clipName, float blendInSpeed = 15.0f, float fadeOutSeconds = 0.1f);
		bool PlayLayerOneShot(size_t index, size_t clipIndex, float blendInSpeed = 15.0f, float fadeOutSeconds = 0.1f);

		// Restrict a layer to rootBone's subtree (e.g. the shoulder so only the arms shoot).
		void SetLayerBoneMask(size_t index, const Bone* rootBone, bool includeRoot = true);
		void ClearLayerBoneMask(size_t index);

		void Update(float deltaTime);
		Math::Matrix4 GetTransform(const Bone* bone) const override;

	private:
		struct Layer
		{
			Animator animator;
			float weight = 0.0f;
			float targetWeight = 0.0f;
			float blendSpeed = 15.0f;

			bool autoFadeOut = false;
			float fadeOutSeconds = 0.1f;

			bool useBoneMask = false;
			std::vector<float> boneMask; // indexed by Bone::index; missing entries count as 0

			float GetBoneWeight(const Bone* bone) const;
		};

		const Animator* mBase = nullptr;
		std::vector<std::unique_ptr<Layer>> mLayers;
	};

	// State-style playback: Play() crossfades from whatever is showing now to a new clip.
	//
	// It owns a tiny pool of Animators that can each play ANY clip, so you never track weights per clip:
	//  - the same clip can fade into itself (rapid-fire shoot animations),
	//  - only a few slots are alive at once, and only while fading out,
	//  - a slot that has faded to 0 is not ticked or sampled.
	// Only the newest ("current") clip raises the finished callback and animation events. An interrupted
	// clip that ends (or fires events) while it fades out is ignored, so "finished" always means
	// "the thing the player most recently asked for is done".
	class AnimationCrossFader final : public IAnimator
	{
	public:
		AnimationCrossFader() = default;
		AnimationCrossFader(const AnimationCrossFader&) = delete; // slots bind `this` for event forwarding
		AnimationCrossFader& operator=(const AnimationCrossFader&) = delete;

		void Initialize(const Model* model);

		// Starts clipIndex and crossfades into it over blendSeconds (0 = instant snap).
		// The very first Play has nothing to fade from, so it is always instant.
		bool Play(size_t clipIndex, bool looping = true, float blendSeconds = 0.15f, bool reverse = false);
		bool Play(const std::string& clipName, bool looping = true, float blendSeconds = 0.15f, bool reverse = false);

		void Update(float deltaTime);
		Math::Matrix4 GetTransform(const Bone* bone) const override;

		bool HasCurrent() const { return mCurrent != kNone; }
		size_t GetCurrentClipIndex() const;
		const Animator* GetCurrentAnimator() const { return mCurrent != kNone ? &mSlots[mCurrent].animator : nullptr; }

		const std::string GetClipNameByIndex(size_t index) const { return mSlots[0].animator.GetClipNameByIndex(index); }
		size_t GetClipIndexByName(const std::string& name) const { return mSlots[0].animator.GetClipIndexByName(name); }

		// Fires once when the CURRENT non-looping clip reaches its end. Never fires for superseded clips.
		FOnAnimationFinished& GetOnCurrentFinishedDelegate() { return mOnCurrentFinished; }
		// Events of the CURRENT clip only.
		FOnAnimationEvent& GetOnAnimationEventDelegate() { return mOnAnimationEvent; }

		// Read-only snapshot of a slot for debug UI.
		struct SlotDebugInfo
		{
			bool active = false;
			bool isCurrent = false;
			size_t clipIndex = 0;
			float weight = 0.0f;
			float targetWeight = 0.0f;
			float elapsedSeconds = 0.0f;
			float durationSeconds = 0.0f;
			bool looping = false;
			bool playing = false;
		};
		size_t GetSlotCount() const { return kMaxSlots; }
		SlotDebugInfo GetSlotDebugInfo(size_t index) const;

	private:
		static constexpr size_t kMaxSlots = 4;
		static constexpr size_t kNone = static_cast<size_t>(-1);

		struct Slot
		{
			Animator animator;
			float weight = 0.0f;
			float targetWeight = 0.0f;
			float fadeSpeed = 0.0f; // weight per second
			bool active = false;    // ticked and sampled while true
			bool notifyOnFinish = false;
			Core::Delegate::FDelegateHandle eventHandle;
		};

		size_t AcquireSlot() const;
		void RefreshSoloCache();
		void ForwardEvent(size_t clipIndex, const std::string& eventName);

		std::array<Slot, kMaxSlots> mSlots;
		size_t mCurrent = kNone;
		size_t mSolo = kNone;         // the only contributing slot, so GetTransform can skip blending
		size_t mUpdatingSlot = kNone; // which slot is mid-Update, so ForwardEvent can filter
		bool mEventsBound = false;

		FOnAnimationFinished mOnCurrentFinished;
		FOnAnimationEvent mOnAnimationEvent;
	};
}