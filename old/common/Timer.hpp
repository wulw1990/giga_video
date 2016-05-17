#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <sys/time.h>
#include <cstdlib>

class Timer
{
public:
	Timer();

	void reset();
	long long getTimeUs();

private:
	struct timeval t1;
};

#endif