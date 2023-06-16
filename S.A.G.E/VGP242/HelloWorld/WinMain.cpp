#include <Core/Inc/Core.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	LOG("Hello, I am %s. My favorite number is %d", "Peter", 42);
	Sleep(1500);
	LOG("%f seconds have passed.", 1.5f);

	const int n = 7;
	ASSERT(n == 7, "42 is not a 7.");

	return 0;
}

