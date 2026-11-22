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
		void OnDisable() override;

		void AddAnimation(std::string animationFileName);
		void AddEvent(const std::string& clipName, float tick, std::string eventName);

		Graphics::Animator& GetAnimator() { return mAnimator; }
		const Graphics::Animator& GetAnimator() const { return mAnimator; }
		Graphics::AnimationBlender& GetBlender() { return mBlender; }
		const Graphics::AnimationBlender& GetBlender() const { return mBlender; }
		const Graphics::IAnimator* GetActiveAnimator() const;

		Graphics::FOnAnimationFinished& GetOnAnimationFinishedDelegate();
		std::string GetAnimationName(size_t index);

	private:
		std::string GetAnimationNameFromFilePath(const std::string& path);
		void LoadAnimatorComponent();
		void LoadAnimationSet(Graphics::Model& model);

		ModelComponent* mModelComponent = nullptr;
		std::vector<std::string> mAnimationFileNames;
		Graphics::AnimationBlender mBlender;
		Graphics::Animator mAnimator;
		bool mHasLoadedAnimationSet = false;

		// Blended Animations
		bool mSupportsBlendedAnimations = false;
		Graphics::FOnAnimationFinished mOnBlendedAnimationFinished;
		void OnBlendedAnimationFinishedCallback(size_t clipIndex);
		std::vector<Core::Delegate::FDelegateHandle> mOnAnimationFinishedHandles;
	};
}