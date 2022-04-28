#ifndef PHM_TIME_H
#define PHM_TIME_H

#include <chrono>

class Time
{
public:
	static float elapsedTime();

	inline float deltaTime() const { return Time::deltaTime_; };

	void updateTime(); // SHOULD ONLY BE RUN ONCE PER FRAME!

private:
	static std::chrono::steady_clock clock_s;
	static std::chrono::steady_clock::time_point startTime_s;

	float deltaTime_ = 0;
	std::chrono::steady_clock::time_point currentTime_ = clock_s.now();
};


#endif /* PHM_TIME_H */