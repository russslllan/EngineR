#include "RusTimer.h"

using namespace std::chrono;

RusTimer::RusTimer()
{
	last = steady_clock::now();
}

float RusTimer::Mark()
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float RusTimer::Peek() const
{
	return duration<float>(steady_clock::now() - last).count();
}
