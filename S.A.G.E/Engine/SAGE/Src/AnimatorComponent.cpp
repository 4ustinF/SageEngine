#include "Precompiled.h"
#include "AnimatorComponent.h"

#include "GameObject.h"
#include "ModelComponent.h"

using namespace SAGE;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(AnimatorComponent, 500);

void AnimatorComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	if (value.HasMember("Supports Blended Animations"))
	{
		mSupportsBlendedAnimations = value["Supports Blended Animations"].GetBool();
	}
}

void AnimatorComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	// --- Supports Blended Animation ---
	if (mSupportsBlendedAnimations)
	{
		SaveBoolToTemplate(compObj, allocator, "Supports Blended Animations", mSupportsBlendedAnimations);
	}
}

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
	mBlender.Update(deltaTime);
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

	if (mSupportsBlendedAnimations)
	{
		const size_t entryCount = mBlender.GetEntryCount();
		mOnAnimationFinishedHandles.reserve(entryCount);

		for (size_t i = 0; i < entryCount; ++i)
		{
			if (Animator* animator = mBlender.GetAnimator(i))
			{
				Core::Delegate::FDelegateHandle onAnimationFinishedHandle = animator->GetOnAnimationFinishedDelegate().AddRaw(this, &AnimatorComponent::OnBlendedAnimationFinishedCallback);
				mOnAnimationFinishedHandles.push_back(std::move(onAnimationFinishedHandle));
			}
		}
	}
}

void AnimatorComponent::OnDisable()
{
	if (mSupportsBlendedAnimations)
	{
		const size_t entryCount = mBlender.GetEntryCount();
		for (size_t i = 0; i < entryCount; ++i)
		{
			if (Animator* animator = mBlender.GetAnimator(i))
			{
				animator->GetOnAnimationFinishedDelegate().Remove(mOnAnimationFinishedHandles[i]);
			}
		}

		mOnAnimationFinishedHandles.clear();
	}
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

const IAnimator& AnimatorComponent::GetActiveAnimator() const
{
	return mSupportsBlendedAnimations ?
		static_cast<const IAnimator&>(mBlender) :
		static_cast<const IAnimator&>(mAnimator);
}

FOnAnimationFinished& AnimatorComponent::GetOnAnimationFinishedDelegate() 
{ 
	if (mSupportsBlendedAnimations)
	{
		return mOnBlendedAnimationFinished;
	}

	return mAnimator.GetOnAnimationFinishedDelegate();
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

	if (mSupportsBlendedAnimations)
	{
		if (mBlender.GetEntryCount() == 0) // No need to reinit and create entrys.
		{
			mBlender.SetBase(&mAnimator);

			const size_t animationCount = model.animationSet.size();
			for (size_t i = 0; i < animationCount; ++i)
			{
				Animator* animator = mBlender.AddAnimator();
				animator->Initialize(&mModelComponent->GetModel()); // call this after the model is loaded/enabled
			}
		}
	}
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

void AnimatorComponent::OnBlendedAnimationFinishedCallback(size_t clipIndex)
{
	mOnBlendedAnimationFinished.Broadcast(clipIndex);
}