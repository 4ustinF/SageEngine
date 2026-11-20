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

		void Initialize() override;

		void Update(float deltaTime) override;
		void DebugUI() override;
		void OnEnable() override;

		void AddAnimation(std::string animationFileName);
		void AddEvent(const std::string& clipName, float tick, std::string eventName);

		Graphics::Animator& GetAnimator() { return mAnimator; }
		const Graphics::Animator& GetAnimator() const { return mAnimator; }
		Graphics::FOnAnimationFinished& GetOnAnimationFinishedDelegate() { return mAnimator.GetOnAnimationFinishedDelegate(); }

		std::string GetAnimationName(size_t index);

	private:
		std::string GetAnimationNameFromFilePath(const std::string& path);
		void LoadAnimatorComponent();
		void LoadAnimationSet(Graphics::Model& model);

		ModelComponent* mModelComponent = nullptr;
		std::vector<std::string> mAnimationFileNames;
		Graphics::Animator mAnimator;
		bool mHasLoadedAnimationSet = false;
	};
}

/*

// .h
Graphics::AnimationBlender& GetBlender() { return mBlender; }
Graphics::Animator* AddBlendLayer();
const Graphics::IAnimator* GetActiveAnimator() const
{
	return mBlender.GetLayerCount() > 0 ? static_cast<const Graphics::IAnimator*>(&mBlender)
										: static_cast<const Graphics::IAnimator*>(&mAnimator);
}
private:
Graphics::AnimationBlender mBlender;

// .cpp
Animator* AnimatorComponent::AddBlendLayer()
{
	mBlender.SetBase(&mAnimator);
	Animator* layer = mBlender.AddLayer();
	layer->Initialize(&mModelComponent->GetModel()); // call this after the model is loaded/enabled
	return layer;
}

void AnimatorComponent::Update(float deltaTime)
{
	mAnimator.Update(deltaTime);
	mBlender.Update(deltaTime);
}

void AnimatorComponent::LoadAnimatorComponent()
{
	Model& model = mModelComponent->GetModel();
	LoadAnimationSet(model);
	mAnimator.Initialize(&model);
	for (size_t i = 0; i < mBlender.GetLayerCount(); ++i)
		mBlender.GetLayer(i)->Initialize(&model);
}

*/