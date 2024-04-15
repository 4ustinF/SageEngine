#include "GameState.h"

using namespace SAGE;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	SAGE::AppConfig appConfig;
	appConfig.appName = L"Flappy Bird";
	appConfig.winWidth = 480;
	appConfig.winHeight = 853;
	appConfig.debugDrawLimit = 3000;
	appConfig.textureRoot = "../../Assets/Images";
	appConfig.audioRoot = "../../Assets/Audio";

	auto& app = MainApp();
	app.AddState<GameState>("GameState");
	app.Run(appConfig);
	return 0;
}
