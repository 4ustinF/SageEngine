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
	SAGE::Graphics::DirectionalLight mDirectionalLight;

	SAGE::Graphics::StandardEffect mStandardEffect;
	SAGE::Graphics::ShadowEffect mShadowEffect;

	SAGE::Graphics::Model mDuckModel;
	SAGE::Graphics::RenderGroup mDuck;
	SAGE::Graphics::RenderObject mGroundRenderObject;
	SAGE::Graphics::RenderObject mClothRenderObject;

	SAGE::Physics::PhysicsWorld mPhysicsWorld;

	std::vector<SAGE::Physics::Particle*> mClothParticles;
	SAGE::Graphics::Mesh mMesh;

	std::vector<SAGE::Physics::Tetrahedron> mTetrahedrons;

	float mFPS = 0.0f;
};