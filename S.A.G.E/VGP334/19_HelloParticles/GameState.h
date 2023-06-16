#include "ParticleSystem.h"

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

	SAGE::Graphics::ParticleEffect mParticleEffect;
	ParticleSystem mParticleSystem;

	bool useMouseDebugMode = true;
	float mFPS = 0.0f;
};