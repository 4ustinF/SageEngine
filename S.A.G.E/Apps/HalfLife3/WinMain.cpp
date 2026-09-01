#include "GameState.h"

using namespace SAGE;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	SAGE::AppConfig appConfig;
	appConfig.appName = L"Half-Life";
	appConfig.winWidth = 1280;
	appConfig.winHeight = 720;
	//appConfig.winWidth = 10;
	//appConfig.winHeight = 10;
	appConfig.debugDrawLimit = 20000;
	appConfig.textureRoot = "../../Assets/Images";
	appConfig.audioRoot = "../../Assets/Audio/Halflife";
	// appConfig.assetRoot = "../../Assets"; // TODO:

	auto& app = MainApp();
	app.AddState<GameState>("GameState");
	app.Run(appConfig);
	return 0;
}
