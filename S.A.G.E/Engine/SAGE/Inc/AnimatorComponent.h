#pragma once

#include "Component.h"

namespace SAGE
{
	class ModelComponent;

	class AnimatorComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::Animator)
		MEMORY_POOL_DECLARE

		const char* GetCompName() override { return "Animator Component"; }
		void LoadComponentFromTemplate(const rapidjson::Value& value) override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;


		void Initialize() override;

		void Update(float deltaTime) override;
		void DebugUI() override;
		void OnEnable() override;

		void AddAnimation(std::string animationFileName);
		void AddEvent(const std::string& clipName, float tick, std::string eventName);

		// Base layer (idle/walk/etc). Existing gameplay code keeps using this.
		Graphics::Animator& GetAnimator() { return mAnimator; }
		const Graphics::Animator& GetAnimator() const { return mAnimator; }
		Graphics::FOnAnimationFinished& GetOnAnimationFinishedDelegate() { return mAnimator.GetOnAnimationFinishedDelegate(); }

		// Overlay layers (shoot, reload, flinch...). Layer indices are the order of AddBlendLayer calls.
		Graphics::Animator* AddBlendLayer();
		Graphics::AnimationBlender& GetBlender() { return mBlender; }

		// State-style crossfading (draw -> idle -> shoot -> reload...). Once enabled it replaces the base
		// animator and layers: drive everything through the returned cross-fader instead.
		Graphics::AnimationCrossFader& EnableCrossFading();
		Graphics::AnimationCrossFader& GetCrossFader() { return mCrossFader; }

		// What the renderer should sample: cross-fader if enabled, else the blender once any layer exists,
		// else the base animator.
		const Graphics::IAnimator* GetActiveAnimator() const
		{
			if (mUseCrossFader)
			{
				return &mCrossFader;
			}
			if (mBlender.GetLayerCount() > 0)
			{
				return &mBlender;
			}
			return &mAnimator;
		}

		std::string GetAnimationName(size_t index);

	private:
		std::string GetAnimationNameFromFilePath(const std::string& path);
		void LoadAnimatorComponent();
		void DrawPlaybackDebug();
		void LoadAnimationSet(Graphics::Model& model);

		ModelComponent* mModelComponent = nullptr;
		std::vector<std::string> mAnimationFileNames;
		Graphics::Animator mAnimator;
		Graphics::AnimationBlender mBlender;
		Graphics::AnimationCrossFader mCrossFader;
		bool mUseCrossFader = false;
		size_t mLoadedFileCount = 0;        // how many entries of mAnimationFileNames were already handled
		bool mLoadsClipsForModel = false;   // true if this component is the one that adds clips to the shared Model
		bool mIsModelReady = false;
	};
}