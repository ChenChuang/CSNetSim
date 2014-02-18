#include "timer.h"

Timer::Timer(Clock* aclock): clock(aclock), time(-1)
{
}

Timer::~Timer()
{
}

void Timer::set_after(double after)
{
	this->time = this->network->clock()->get_time() + after;
}

void Timer::is_timeout()
{
	result = (this->time >= 0 && this->time <= this->network->clock()->get_time());
	this->time = -1;
	return result;
}