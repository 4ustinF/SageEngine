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

	SAGE::Math::Vector3 mPosition = SAGE::Math::Vector3::Zero;
	SAGE::Graphics::Color mColor = SAGE::Graphics::Colors::White;

	float mFPS = 0.0f;
};