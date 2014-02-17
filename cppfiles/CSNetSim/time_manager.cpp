#include "time_manager.h"

TimeManager::TimeManager(): time(atime), tick(atick), default_tick(atick)
{
}

TimeManager::~TimeManager()
{
}

TimeManager::try_set_tick(double atick)
{
	if(atick > 0 && (atick < this->tick || this->tick <= 0)){
		this->tick = atick;
	}
}

TimeManager::tick_setter_init()
{
	this->zero_tick();
}

TimeManager::tick_setter_exit()
{
	this->check_tick();
}

TimeManager::check_tick()
{
	if(this->tick <= 0){
		this->tick = this->default_tick;
	}
}

TimeManager::ticktock()
{
	this->time += this->tick;
}
