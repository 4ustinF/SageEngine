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
	if (ImGui::CollapsingHeader("Animator Component##AnimatorComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		// TODO: Play and pause buttons
		const float spacing = ImGui::GetStyle().ItemSpacing.x;
		const ImVec2 playButtonSize = ImVec2(50.0f, 0);
		const ImVec2 oneShotButtonSize = ImVec2(70.0f, 0);

		for (int i = 0; i < mAnimationFileNames.size(); ++i)
		{
			const std::string animationName = GetAnimationName(mAnimationFileNames[i]);
			ImGui::PushID(animationName.c_str());
			ImGui::Text("%s", animationName.c_str());

			const float x = ImGui::GetContentRegionAvail().x - playButtonSize.x - oneShotButtonSize.x - spacing;
			ImGui::SameLine(ImGui::GetCursorPosX() + x);

			if (ImGui::Button("Play", playButtonSize))
			{
				mAnimator.PlayAnimation(i + 1, true);
			}

			ImGui::SameLine();

			if (ImGui::Button("One Shot", oneShotButtonSize))
			{
				mAnimator.PlayAnimation(i + 1, false);
			}

			ImGui::PopID();
		}
	}
}

void AnimatorComponent::AddAnimation(std::string animationFileName)
{
	mAnimationFileNames.emplace_back(std::move(animationFileName));
}


std::string AnimatorComponent::GetAnimationName(int index)
{
	const std::string& path = mAnimationFileNames[index];
	return GetAnimationName(path);
}

std::string AnimatorComponent::GetAnimationName(const std::string& path)
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
