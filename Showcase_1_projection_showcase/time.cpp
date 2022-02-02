#include "pch.h"

#include "time.h"


void Time::updateTime()
{
	std::chrono::steady_clock::time_point now = clock_.now();
	deltaTime_ = std::chrono::duration<float, std::chrono::seconds::period>(now - current_time_).count();
	current_time_ = now;
}
