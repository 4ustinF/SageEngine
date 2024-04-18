#include <SAGE/Inc/SAGE.h>

class PipeComponent;

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
	SAGE::Graphics::TextureId mPipeTextureID = 0;

	float mBase1XPos = 0.0f;
	float mBase2XPos = 480.0f;
	float mMoveSpeed = 100.0f;

	PipeComponent* mPipe1 = nullptr;
	PipeComponent* mPipe2 = nullptr;
	PipeComponent* mPipe3 = nullptr;
};