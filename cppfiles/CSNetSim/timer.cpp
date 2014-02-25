#include "timer.h"

Timer::Timer(Clock* aclock): clock(aclock), time(-1)
{
}

Timer::~Timer()
{
}

void Timer::set_after(double after)
{
	this->time = this->clock->get_time() + after;
	this->clock->try_set_tick(after);
}

bool Timer::is_timeout()
{
	bool result = (this->time >= 0 && this->time <= this->clock->get_time());
	if(result){
		this->time = -1;
	}
	return result;
}

void Timer::reset()
{
	this->time = -1;
}