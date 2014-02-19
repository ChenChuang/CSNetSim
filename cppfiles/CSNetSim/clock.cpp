#include "clock.h"

Clock::Clock(double atime, double atick): time(atime), tick(atick), default_tick(atick)
{
}

Clock::~Clock()
{
}

void Clock::try_set_tick(double atick)
{
	if(atick > 0 && (atick < this->tick || this->tick <= 0)){
		this->tick = atick;
	}
}

void Clock::tick_setter_init()
{
	this->zero_tick();
}

void Clock::tick_setter_exit()
{
	this->check_tick();
}

void Clock::check_tick()
{
	if(this->tick <= 0){
		this->tick = this->default_tick;
	}
}

void Clock::ticktock()
{
	this->time += this->tick;
}
