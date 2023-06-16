#include <SAGE/Inc/SAGE.h>
#include "CoroBehaviour.h"

class GameState : public SAGE::AppState
{
public:
	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	Enumerator MoveCubeLeftTask(float Param);
	Enumerator MoveCubeRightTask(float Param);

	CoroBehaviour mCoroBehaviour;

	SAGE::GameWorld mGameWorld;
	SAGE::Math::Vector3 mBoxPos;
};
