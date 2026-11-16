#include "Precompiled.h"
#include "AnimatorComponent.h"

#include "GameObject.h"
#include "ModelComponent.h"

using namespace SAGE;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(AnimatorComponent, 500);

void AnimatorComponent::Initialize()
{
	mModelComponent = GetOwner().GetComponent<ModelComponent>();
	if (mModelComponent->GetIsPreloadingModel())
	{
		LoadAnimatorComponent();
	}
}

void AnimatorComponent::Update(float deltaTime)
{
	mAnimator.Update(deltaTime);
}

void AnimatorComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Animator Component##AnimatorComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		const float spacing = ImGui::GetStyle().ItemSpacing.x;
		const ImVec2 playButtonSize = ImVec2(50.0f, 0);
		const ImVec2 oneShotButtonSize = ImVec2(70.0f, 0);

		for (size_t i = 0; i < mAnimationFileNames.size(); ++i)
		{
			const std::string animationName = GetAnimationName(i);
			ImGui::PushID(animationName.c_str());
			ImGui::Text("%s", animationName.c_str());

			const float x = ImGui::GetContentRegionAvail().x - playButtonSize.x - oneShotButtonSize.x - spacing;
			ImGui::SameLine(ImGui::GetCursorPosX() + x);

			if (ImGui::Button("Play", playButtonSize))
			{
				mAnimator.PlayAnimation(i, true);
			}

			ImGui::SameLine();

			if (ImGui::Button("One Shot", oneShotButtonSize))
			{
				mAnimator.PlayAnimation(i, false);
			}

			ImGui::PopID();
		}
	}
}

void AnimatorComponent::OnEnable()
{
	LoadAnimatorComponent();
	mAnimator.PlayAnimation(mAnimator.GetClipIndex(), mAnimator.GetIsLooping());
}

void AnimatorComponent::AddAnimation(std::string animationFileName)
{
	mAnimationFileNames.emplace_back(std::move(animationFileName));
}

void AnimatorComponent::AddEvent(const std::string& clipName, float tick, std::string eventName)
{
	const size_t index = mAnimator.GetClipIndexByName(clipName);
	if (index == static_cast<size_t>(-1))
	{
		return;
	}

	auto& events = mModelComponent->GetModel().animationSet[index].events;
	for (const AnimationEvent& e : events)
	{
		if (e.tick == tick && e.name == eventName)
		{
			return; // already registered
		}
	}

	events.push_back({ tick, std::move(eventName) });
}

std::string AnimatorComponent::GetAnimationName(size_t index)
{
	std::string name = mAnimator.GetClipNameByIndex(index);
	if (!name.empty())
	{
		return name;
	}

	const std::string& path = mAnimationFileNames[index];
	return GetAnimationNameFromFilePath(path); // Fallback to filename if the clip name is empty.
}

std::string AnimatorComponent::GetAnimationNameFromFilePath(const std::string& path)
{
	const size_t slash = path.find_last_of("/\\");

	// Get filename without directory
	std::string filename = path.substr(slash + 1);

	// Remove ".animset"
	const std::string extension = ".animset";

	if (filename.size() >= extension.size() &&
		filename.compare(filename.size() - extension.size(),
		extension.size(),
		extension) == 0)
	{
		filename.erase(filename.size() - extension.size());
	}

	return filename;
}

void AnimatorComponent::LoadAnimatorComponent()
{
	Model& model = mModelComponent->GetModel();

	LoadAnimationSet(model);
	mAnimator.Initialize(&model);
}

void AnimatorComponent::LoadAnimationSet(Model& model)
{
	if (mHasLoadedAnimationSet)
	{
		return;
	}

	if (model.animationSet.size() == 1)
	{
		for (auto& animationFileName : mAnimationFileNames) {
			ModelIO::LoadAnimationSet(animationFileName, model);
		}
	}

	mHasLoadedAnimationSet = true;
}