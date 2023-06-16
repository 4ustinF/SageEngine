#include <SAGE/Inc/SAGE.h>

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

	SAGE::Graphics::StandardEffect mStandardEffect;
	SAGE::Graphics::ShadowEffect mShadowEffect;

	SAGE::Graphics::SoundId mBgMusicID;

	void CreateArm();
	SAGE::Graphics::RenderObject mArmRenderObject;
	SAGE::Math::Quaternion mArmRotation;

	SAGE::Graphics::RenderObject mItemInHandRenderObject;
	SAGE::Math::Quaternion mItemInHandRotation;
	SAGE::Graphics::RenderObject mBlockInHandRenderObject;

	//SAGE::Graphics::RenderGroup mCharacter;
	//SAGE::Graphics::Model mCharacterModel;
	//SAGE::Graphics::AnimationBlender mCharacterAnimator;
};