#include "GameState.h"

using namespace SAGE;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	auto& app = MainApp();
	app.AddState<GameState>("GameState");
	app.Run({ L"Hello Earth", 1280, 720 });
	return 0;
}
