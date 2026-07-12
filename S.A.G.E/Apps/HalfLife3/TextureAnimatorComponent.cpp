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
		mAnimationFrameFilePaths.clear();
		mAnimationFrames.clear();
		auto tm = TextureManager::Get();

		for(const auto& filePath : filePaths)
		{
			const std::string textureFilePath = filePath.GetString();
			mAnimationFrameFilePaths.push_back(textureFilePath);

			const Graphics::TextureId textureId = tm->LoadTexture(textureFilePath);
			mAnimationFrames.push_back(textureId);
		}
	}

	if (value.HasMember("Animation Time"))
	{
		mAnimationTime = value["Animation Time"].GetFloat();
	}

	if (value.HasMember("Play Immediately"))
	{
		mPlayImmediately = value["Play Immediately"].GetBool();
	}
}

void TextureAnimatorComponent::PreSaveComponentToTemplate()
{
	RestoreOriginalTexture(); // To not override the save of mesh renderer component's texture id.
}

void TextureAnimatorComponent::SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
	SaveStringsToTemplate(compObj, allocator, "TextureId File Paths", mAnimationFrameFilePaths);
	SaveNumberToTemplate(compObj, allocator, "Animation Time", mAnimationTime);
	if (mPlayImmediately == false)
	{
		SaveBoolToTemplate(compObj, allocator, "Play Immediately", mPlayImmediately);
	}
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
	mAnimationFrameFilePaths.clear();
	mAnimationFrames.clear();
}

void TextureAnimatorComponent::OnQueueUpdate(float deltaTime)
{
	mElapsedTime += deltaTime;
	if (mElapsedTime >= mAnimationTime)
	{
		mElapsedTime -= mAnimationTime;
		CycleTexture();
	}

	if(mIsPlaying)
	{
		EnqueueUpdate();
	}
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
