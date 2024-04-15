#include <SAGE/Inc/SAGE.h>

class PlayerAnimatorComponent;

class GameState : public SAGE::AppState
{
public:
	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	SAGE::GameWorld mGameWorld;
	SAGE::Input::InputSystem* mInputSystem = nullptr;
	bool mIsPaused = false;

	SAGE::Graphics::SpriteRenderer* mSpriteRenderer = nullptr;
	SAGE::Graphics::TextureId mBackgroundTextureID = 0;
	SAGE::Graphics::TextureId mBaseTextureID = 0;
};