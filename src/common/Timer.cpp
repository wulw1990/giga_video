#include "Timer.hpp"

Timer::Timer() {
	reset();
}
void Timer::reset() {
	gettimeofday(&t1, NULL);
}
long long Timer::getTimeUs() {
	struct timeval t2;
	gettimeofday(&t2, NULL);
	// cout << "t_.tv_sec: " << t_.tv_sec << endl;
	// cout << "t_.tv_usec: " << t_.tv_usec << endl;
	return (t2.tv_sec - t1.tv_sec) * 1e6 + (t2.tv_usec - t1.tv_usec);
}