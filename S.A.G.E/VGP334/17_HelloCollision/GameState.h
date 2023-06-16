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
	SAGE::Graphics::Camera mCamera;

	SAGE::Math::AABB aabb1;
	SAGE::Math::AABB aabb2;
	SAGE::Math::OBB obb1;
	SAGE::Math::OBB obb2;
	SAGE::Math::Pyramid pyramid1;
	SAGE::Math::Pyramid pyramid2;


	float mFPS = 0.0f;
	bool mShowFilled = false;
};