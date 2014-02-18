#include "clock.h"

Clock::Clock(): time(atime), tick(atick), default_tick(atick)
{
}

Clock::~Clock()
{
}

Clock::try_set_tick(double atick)
{
	if(atick > 0 && (atick < this->tick || this->tick <= 0)){
		this->tick = atick;
	}
}

Clock::tick_setter_init()
{
	this->zero_tick();
}

Clock::tick_setter_exit()
{
	this->check_tick();
}

Clock::check_tick()
{
	if(this->tick <= 0){
		this->tick = this->default_tick;
	}
}

Clock::ticktock()
{
	this->time += this->tick;
}
