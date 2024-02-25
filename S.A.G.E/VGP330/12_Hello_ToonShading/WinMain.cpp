#include "GameState.h"

using namespace SAGE;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	SAGE::AppConfig appConfig;
	appConfig.appName = L"Hello Toon Shading";
	appConfig.winWidth = 1280;
	appConfig.winHeight = 720;
	appConfig.debugDrawLimit = 1000000;
	appConfig.textureRoot = "../../Assets/Images";

	auto& app = MainApp();
	app.AddState<GameState>("GameState");
	app.Run(appConfig);
	return 0;
}