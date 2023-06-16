#include "Precompiled.h"
#include "SAGE.h"

SAGE::App& SAGE::MainApp()
{
	static App sApp;
	return sApp;
}