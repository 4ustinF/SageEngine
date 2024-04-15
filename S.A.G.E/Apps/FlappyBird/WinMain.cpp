#include "GameState.h"

using namespace SAGE;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	SAGE::AppConfig appConfig;
	appConfig.appName = L"Flappy Bird";
	appConfig.winWidth = 672;
	appConfig.winHeight = 864;
	appConfig.debugDrawLimit = 3000;
	appConfig.textureRoot = "../../Assets/Images";
	appConfig.audioRoot = "../../Assets/Audio/PacMan";

	auto& app = MainApp();
	app.AddState<GameState>("GameState");
	app.Run(appConfig);
	return 0;
}
