#include "timer.h"

Timer::Timer(Network* anetwork): network(anetwork), time(0)
{
}

Timer::~Timer()
{
}

void Timer::set_after(double after)
{
	this->time = network->get_time() + after;
}

void Timer::is_timeout()
{
	result = this->time <= network->get_time();
	this->time = 0;
	return result;
}