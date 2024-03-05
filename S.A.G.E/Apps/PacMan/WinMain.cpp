#include "GameState.h"

using namespace SAGE;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	SAGE::AppConfig appConfig;
	appConfig.appName = L"PacMan";
	appConfig.winWidth = 672;
	appConfig.winHeight = 744;
	appConfig.debugDrawLimit = 3000;
	appConfig.textureRoot = "../../Assets/Images";
	appConfig.audioRoot = "../../Assets/Audio";

	auto& app = MainApp();
	app.AddState<GameState>("GameState");
	app.Run(appConfig);
	return 0;
}