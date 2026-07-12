#pragma once

#include "TypeIds.h"

class SAGE::MeshRendererComponent;

class TextureAnimatorComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::TextureAnimator);
	MEMORY_POOL_DECLARE;

	const char* GetCompName() override { return "Texture Animator Component"; }
	void LoadComponentFromTemplate(const rapidjson::Value& value) override;
	void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

	void Initialize() override;
	void Terminate() override;

	void OnQueueUpdate(float deltaTime) override;
	void DebugUI() override;

	void OnEnable() override;

private:
	void RestoreOriginalTexture();
	void CycleTexture();
	void PlayCycle();
	void PauseCycle();

	SAGE::MeshRendererComponent* mMeshRendererComponent = nullptr;
	float mElapsedTime = 0.0f;
	float mAnimationTime = 1.0f;
	bool mIsPlaying = true;
	bool mPlayImmediately = true;

	SAGE::Graphics::TextureId mRenderObjectDiffuseMapId = 0;
	std::vector<SAGE::Graphics::TextureId> mAnimationFrames;
	int mCurrentFrameIndex = 0;
};