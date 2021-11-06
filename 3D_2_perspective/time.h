#ifndef PHM_TIME_H
#define PHM_TIME_H
#include <chrono>

class Time
{
public:
	static inline float deltaTime() { return Time::s_deltaTime_; };

	static void updateTime(); // SHOULD ONLY BE RUN ONCE PER FRAME!

private:
	static std::chrono::steady_clock clock;

	static float s_deltaTime_;
	static std::chrono::steady_clock::time_point s_current_time_;
};


#endif /* PHM_TIME_H */