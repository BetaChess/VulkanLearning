#include "pch.h"

#include "time.h"

std::chrono::steady_clock Time::clock_s = std::chrono::steady_clock();
std::chrono::steady_clock::time_point Time::startTime_s = clock_s.now();

float Time::elapsedTime()
{
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(clock_s.now() - startTime_s);
	return static_cast<float>(duration.count()) / 1000.0f;
	
	//return (float)std::chrono::duration_cast<std::chrono::milliseconds>(clock_s.now() - startTime_s).count() / 1000.0f;
}

void Time::updateTime()
{
	std::chrono::steady_clock::time_point now = clock_s.now();
	deltaTime_ = std::chrono::duration<float, std::chrono::seconds::period>(now - currentTime_).count();
	currentTime_ = now;
}
