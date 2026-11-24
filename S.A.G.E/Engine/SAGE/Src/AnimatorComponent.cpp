#include "Precompiled.h"
#include "AnimatorComponent.h"

#include "GameObject.h"
#include "ModelComponent.h"

using namespace SAGE;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(AnimatorComponent, 500);

namespace
{
	// One line describing what a single Animator is playing.
	void DrawAnimatorLine(const Animator& animator)
	{
		if (animator.GetModel() == nullptr)
		{
			ImGui::TextDisabled("Not initialized");
			return;
		}

		std::string name = animator.GetClipNameByIndex(animator.GetClipIndex());
		if (name.empty())
		{
			name = "Clip " + std::to_string(animator.GetClipIndex());
		}

		ImGui::Text("%s  |  %.2f / %.2fs  |  %s%s",
			name.c_str(),
			animator.GetElapsedSeconds(),
			animator.GetDurationSeconds(),
			animator.GetIsLooping() ? "Looping" : "One-shot",
			animator.IsPlaying() ? "" : "  (stopped)");
	}

	// Pass a negative targetWeight to show only the current weight.
	void DrawWeightBar(float weight, float targetWeight = -1.0f)
	{
		char overlay[48];
		if (targetWeight < 0.0f) { snprintf(overlay, sizeof(overlay), "%.2f", weight); }
		else { snprintf(overlay, sizeof(overlay), "%.2f -> %.2f", weight, targetWeight); }
		ImGui::ProgressBar(weight, ImVec2(-1.0f, 0.0f), overlay);
	}
}

void AnimatorComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	if (value.HasMember("AnimSets"))
	{
		const auto& animSets = value["AnimSets"].GetArray();
		for (auto& animset : animSets)
		{
			AddAnimation(animset.GetString());
		}
	}
}

void AnimatorComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	// TODO: 
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
	if (mUseCrossFader)
	{
		mCrossFader.Update(deltaTime); // replaces the base animator and layers
		return;
	}

	mAnimator.Update(deltaTime);
	mBlender.Update(deltaTime); // ticks overlay layers only; the base is ticked above
}

void AnimatorComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Animator Component##AnimatorComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		DrawPlaybackDebug();
		ImGui::Separator();

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
				if (mUseCrossFader) { mCrossFader.Play(i, true); }
				else { mAnimator.PlayAnimation(i, true); }
			}

			ImGui::SameLine();

			if (ImGui::Button("One Shot", oneShotButtonSize))
			{
				if (mUseCrossFader) { mCrossFader.Play(i, false); }
				else { mAnimator.PlayAnimation(i, false); }
			}

			ImGui::PopID();
		}
	}
}

void AnimatorComponent::DrawPlaybackDebug()
{
	if (!ImGui::TreeNodeEx("Playback##AnimatorComponent", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	if (!mIsModelReady)
	{
		ImGui::TextDisabled("Model not loaded yet");
	}
	else if (mUseCrossFader)
	{
		// Slots with weight > 0 all contribute to the pose. More than one means a blend is in progress.
		int contributing = 0;
		const size_t slotCount = mCrossFader.GetSlotCount();
		for (size_t i = 0; i < slotCount; ++i)
		{
			const AnimationCrossFader::SlotDebugInfo info = mCrossFader.GetSlotDebugInfo(i);
			if (info.active && info.weight > 0.0f)
			{
				++contributing;
			}
		}
		ImGui::Text("Mode: Cross-Fader  |  Blending: %s (%d contributing)", contributing > 1 ? "yes" : "no", contributing);

		for (size_t i = 0; i < slotCount; ++i)
		{
			const AnimationCrossFader::SlotDebugInfo info = mCrossFader.GetSlotDebugInfo(i);
			ImGui::PushID(static_cast<int>(i));

			if (!info.active)
			{
				ImGui::TextDisabled("Slot %d: unused", static_cast<int>(i));
			}
			else
			{
				std::string name = mCrossFader.GetClipNameByIndex(info.clipIndex);
				if (name.empty())
				{
					name = "Clip " + std::to_string(info.clipIndex);
				}

				ImGui::Text("Slot %d%s: %s  |  %.2f / %.2fs  |  %s%s",
					static_cast<int>(i),
					info.isCurrent ? " [CURRENT]" : "",
					name.c_str(),
					info.elapsedSeconds,
					info.durationSeconds,
					info.looping ? "Looping" : "One-shot",
					info.playing ? "" : "  (stopped)");
				DrawWeightBar(info.weight, info.targetWeight);
			}

			ImGui::PopID();
		}
	}
	else if (mBlender.GetLayerCount() > 0)
	{
		ImGui::Text("Mode: Base + Layers");
		ImGui::Text("Base:");
		DrawAnimatorLine(mAnimator);

		for (size_t i = 0; i < mBlender.GetLayerCount(); ++i)
		{
			ImGui::PushID(static_cast<int>(i));
			ImGui::Text("Layer %d:", static_cast<int>(i));
			DrawAnimatorLine(*mBlender.GetLayer(i));
			DrawWeightBar(mBlender.GetLayerWeight(i));
			ImGui::PopID();
		}
	}
	else
	{
		ImGui::Text("Mode: Single Animator");
		DrawAnimatorLine(mAnimator);
	}

	ImGui::TreePop();
}

void AnimatorComponent::OnEnable()
{
	LoadAnimatorComponent();
	mAnimator.PlayAnimation(mAnimator.GetClipIndex(), mAnimator.GetIsLooping());
}

void AnimatorComponent::AddAnimation(std::string animationFileName)
{
	mAnimationFileNames.emplace_back(std::move(animationFileName));

	// Added after the model was already set up (e.g. from code after OnEnable): load it now,
	// otherwise it would never be loaded and the clip index would be out of range.
	if (mIsModelReady)
	{
		LoadAnimatorComponent();
	}
}

Animator* AnimatorComponent::AddBlendLayer()
{
	mBlender.SetBase(&mAnimator);
	Animator* layer = mBlender.AddLayer();

	// If the model isn't loaded yet, LoadAnimatorComponent will initialize this layer later.
	if (mIsModelReady)
	{
		layer->Initialize(&mModelComponent->GetModel());
	}
	return layer;
}

AnimationCrossFader& AnimatorComponent::EnableCrossFading()
{
	mUseCrossFader = true;

	// If the model isn't loaded yet, LoadAnimatorComponent will initialize it later.
	if (mIsModelReady)
	{
		mCrossFader.Initialize(&mModelComponent->GetModel());
	}
	return mCrossFader;
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

	for (size_t i = 0; i < mBlender.GetLayerCount(); ++i)
	{
		mBlender.GetLayer(i)->Initialize(&model); // no-op if already initialized with this model
	}
	if (mUseCrossFader)
	{
		mCrossFader.Initialize(&model); // no-op for slots already initialized with this model
	}
	mIsModelReady = true;
}

void AnimatorComponent::LoadAnimationSet(Model& model)
{
	if (mLoadedFileCount >= mAnimationFileNames.size())
	{
		return; // nothing new to load (also true while the list is still empty, so we retry later)
	}

	// A shared Model starts with just its base clip. Only the first component to see it that way loads the
	// extra clips; later instances of the same model reuse them instead of loading duplicates.
	if (model.animationSet.size() == 1)
	{
		mLoadsClipsForModel = true;
	}

	if (mLoadsClipsForModel)
	{
		for (size_t i = mLoadedFileCount; i < mAnimationFileNames.size(); ++i)
		{
			ModelIO::LoadAnimationSet(mAnimationFileNames[i], model);
		}
	}

	mLoadedFileCount = mAnimationFileNames.size();
}