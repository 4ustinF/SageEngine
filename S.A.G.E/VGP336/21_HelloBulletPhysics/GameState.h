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
	btRigidBody* mPlayerRB = nullptr;
	btRigidBody* mGroundRB = nullptr;
	btRigidBody* mCustomRB = nullptr;
};