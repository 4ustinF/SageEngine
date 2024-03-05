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
	PlayerAnimatorComponent* mPlayerAnimatorComponent = nullptr;
};