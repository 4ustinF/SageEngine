#include "GameState.h"
#include "HeartState.h"
#include "SwordState.h"

using namespace SAGE;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	auto& app = MainApp();
	app.AddState<GameState>("GameState");
	app.AddState<HeartState>("HeartState");
	app.AddState<SwordState>("SwordState");
	app.Run({ L"Hello Shapes", 1280, 720 });
	return 0;
}
