#include "Precompiled.h"
#include "AnimatorComponent.h"

#include "GameObject.h"
#include "ModelComponent.h"

using namespace SAGE;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(AnimatorComponent, 1000);

void AnimatorComponent::Initialize()
{
	mModelComponent = GetOwner().GetComponent<ModelComponent>();

	Model& model = mModelComponent->GetModel();
	if (model.animationSet.size() == 1)
	{
		for (auto& animationFileName : mAnimationFileNames) {
			ModelIO::LoadAnimationSet(animationFileName, model);
		}
	}

	mAnimator.Initialize(&model);
	mAnimator.PlayAnimation(0);
}

void AnimatorComponent::Update(float deltaTime)
{
	mAnimator.Update(deltaTime);
}

void AnimatorComponent::DebugUI()
{
	// TODO
}

void AnimatorComponent::AddAnimation(std::string animationFileName)
{
	mAnimationFileNames.emplace_back(std::move(animationFileName));
}