#include "TextureAnimatorComponent.h"

#include "SAGE/Inc/MeshRendererComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(TextureAnimatorComponent, 100);

void TextureAnimatorComponent::LoadComponentFromTemplate(const rapidjson::Value& value)
{
	if (value.HasMember("TextureId File Paths"))
	{
		const auto& filePaths = value["TextureId File Paths"].GetArray();

		for(const auto& filePath : filePaths)
		{
			const std::string textureFilePath = filePath.GetString();
			auto tm = TextureManager::Get();
			const Graphics::TextureId textureId = tm->LoadTexture(textureFilePath);
			mAnimationFrames.push_back(textureId);
		}
	}

	if (value.HasMember("Animation Time"))
	{
		mAnimationTime = value["Animation Time"].GetFloat();
	}
}

void TextureAnimatorComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	// TODO:
}

void TextureAnimatorComponent::Initialize()
{
	mMeshRendererComponent = GetOwner().GetComponent<MeshRendererComponent>();
	if (mPlayImmediately)
	{
		PlayCycle();
	}
}

void TextureAnimatorComponent::Terminate()
{
	mMeshRendererComponent = nullptr;
}

void TextureAnimatorComponent::OnQueueUpdate(float deltaTime)
{
	mElapsedTime += deltaTime;
	if (mElapsedTime >= mAnimationTime)
	{
		mElapsedTime -= mAnimationTime;
		CycleTexture();
	}

	mQueueUpdate = mIsPlaying;
}

void TextureAnimatorComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Texture Animator Component##TextureAnimatorComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Checkbox("Play Immediately##TextureAnimatorComponent", &mPlayImmediately);
		if (ImGui::DragFloat("Animation Speed##TextureAnimatorComponent", &mAnimationTime, 0.1f, 0.1f, 10.0f))
		{
			mElapsedTime = 0.0f;
			mAnimationTime = std::max(0.1f, mAnimationTime);
		}

		if (mIsPlaying)
		{
			if (ImGui::Button("Pause Cycle##TextureAnimatorComponent"))
			{
				PauseCycle();
			}
		}
		else if (ImGui::Button("Play Cycle##TextureAnimatorComponent"))
		{
			PlayCycle();
		}

		if (ImGui::Button("Restore To Original Texture##TextureAnimatorComponent"))
		{
			RestoreOriginalTexture();
		}
	}

	// TODO: Let me select textures via here.
}

void TextureAnimatorComponent::OnEnable()
{
	mRenderObjectDiffuseMapId = mMeshRendererComponent ? mMeshRendererComponent->GetRenderObject().diffuseMapId : 0;
}

void TextureAnimatorComponent::RestoreOriginalTexture()
{
	if (mMeshRendererComponent != nullptr)
	{
		mMeshRendererComponent->GetRenderObject().diffuseMapId = mRenderObjectDiffuseMapId;
	}
}

void TextureAnimatorComponent::CycleTexture()
{
	if (mMeshRendererComponent == nullptr)
	{
		return;
	}

	const int animationFramesSize = static_cast<int>(mAnimationFrames.size());
	if (animationFramesSize == 0)
	{
		return;
	}

	if (++mCurrentFrameIndex >= animationFramesSize)
	{
		mCurrentFrameIndex = 0;
	}

	mMeshRendererComponent->GetRenderObject().diffuseMapId = mAnimationFrames[mCurrentFrameIndex];
}

void TextureAnimatorComponent::PlayCycle()
{
	mIsPlaying = true;
	EnqueueUpdate();
}

void TextureAnimatorComponent::PauseCycle()
{
	mIsPlaying = false;
	mElapsedTime = 0.0f;
	RestoreOriginalTexture();
}
