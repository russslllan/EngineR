#pragma once
#include <chrono>

class RusTimer
{
public:
	RusTimer();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};
