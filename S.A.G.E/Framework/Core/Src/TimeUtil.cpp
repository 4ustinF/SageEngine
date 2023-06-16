#include "Precompiled.h"
#include "TimeUtil.h"

float SAGE::Core::TimeUtil::GetTime()
{
	// Get the time difference since the start time
	static const auto startTime = std::chrono::high_resolution_clock::now();
	const auto currentTime = std::chrono::high_resolution_clock::now();
	const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
	return milliseconds * 0.001f;
}

float SAGE::Core::TimeUtil::GetDeltaTime()
{
	// Get the time difference since the last time we called this function
	static auto lastCallTime = std::chrono::high_resolution_clock::now();
	const auto currentTime = std::chrono::high_resolution_clock::now();
	const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastCallTime).count();
	lastCallTime = currentTime;
	return milliseconds * 0.001f;
}
