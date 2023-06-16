#include <SAGE/Inc/SAGE.h>

class GameState : public SAGE::AppState
{
public:
	enum Direction
	{
		Idle,
		Forward,
		ForwardRight,
		Right,
		BackwardRight,
		Backward, 
		BackwardLeft,
		Left,
		ForwardLeft,
		Count
	};

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	SAGE::Graphics::Camera mCamera;
	SAGE::Graphics::DirectionalLight mDirectionalLight;

	SAGE::Graphics::StandardEffect mStandardEffect;
	SAGE::Graphics::ShadowEffect mShadowEffect;

	SAGE::Graphics::Model mCharacterModel;
	//SAGE::Graphics::Animator mCharacterAnimator;
	SAGE::Graphics::AnimationBlender mCharacterAnimator;

	SAGE::Graphics::RenderGroup mCharacter;
	SAGE::Graphics::RenderObject mGroundRenderObject;

	bool mDrawSkeleton = false;
	float mFPS = 0.0f;
	float mSpeedUp = 0.0f;
	float mSpeedDown = 0.0f;
	float mSpeedRight = 0.0f;
	float mSpeedLeft = 0.0f;

	float idleSpeed = 0.0f;
};