#include "time.h"


float Time::s_deltaTime_ = 0;
std::chrono::steady_clock::time_point Time::s_current_time_ = Time::clock.now();

void Time::updateTime()
{
	std::chrono::steady_clock::time_point now = clock.now();
	auto delta = (now - s_current_time_);
	s_deltaTime_ = (delta.count() / 1000000000.f);
	s_current_time_ = now;
}
