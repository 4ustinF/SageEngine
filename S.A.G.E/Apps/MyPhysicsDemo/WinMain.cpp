#include "GameState.h"

using namespace SAGE;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	SAGE::AppConfig appConfig;
	appConfig.appName = L"My Physics Demo";
	appConfig.winWidth = 10;
	//appConfig.winWidth = 1280;
	appConfig.winHeight = 10;
	//appConfig.winHeight = 720;
	appConfig.debugDrawLimit = 100000;
	appConfig.textureRoot = "../../Assets/Images";
	appConfig.audioRoot = "../../Assets/Audio";

	auto& app = MainApp();
	app.AddState<GameState>("GameState");
	app.Run(appConfig);
	return 0;
}
