#pragma once

#include "TypeIds.h"

class PlayerControllerComponent;

class PlayerAnimatorComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::PlayerAnimator);
	MEMORY_POOL_DECLARE;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;

	void Render();

private:
	PlayerControllerComponent* mPlayerController = nullptr;
	SAGE::Graphics::TextureId mTextureID = 0;
};