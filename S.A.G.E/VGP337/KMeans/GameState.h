#include <SAGE/Inc/SAGE.h>
#include <ImGui/Inc/AppLog.h>

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
	AppLog mAppLog;

	struct Unit
	{
		ImVec2 position;
	};

	SAGE::Graphics::Texture mUnitTexture;
	std::vector<Unit> mUnits;
};