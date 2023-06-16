#include <Core/Inc/Core.h>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	SAGE::Core::Window mywindow;
	mywindow.Initialize(instance, L"Hello Window", 1280, 720);

	while (true)
	{
		mywindow.ProcessMessage();
		if (!mywindow.IsActive()) {
			break;
		}

		if (GetAsyncKeyState(VK_ESCAPE)) {
			break;
		}
	}

	mywindow.Terminate();
	return 0;
}