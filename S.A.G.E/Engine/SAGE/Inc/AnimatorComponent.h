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

		void AddAnimation(std::string animationFileName);

		Graphics::Animator& GetAnimator() { return mAnimator; }
		const Graphics::Animator& GetAnimator() const { return mAnimator; }

	private:
		ModelComponent* mModelComponent = nullptr;

		std::vector<std::string> mAnimationFileNames;

		Graphics::Animator mAnimator;
	};
}