#include "clock.h"

Clock::Clock(double time, double default_tick, double min_tick): 
	time(time), tick(default_tick), default_tick(default_tick), min_tick(min_tick)
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
	if(this->tick > 0 && this->tick < this->min_tick){
		//this->tick = this->min_tick;
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
