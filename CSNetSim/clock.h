#ifndef CLOCK_H
#define CLOCK_H

#include "compile_config.h"

class Clock
{
public:
	Clock(double time, double default_tick, double min_tick);
	~Clock();
public:
	double get_time() {return this->time;}
	double get_tick() {return this->tick;}
	void try_set_tick(double atick);
	void tick_setter_init();
	void tick_setter_exit();
private:
	void set_tick(double atick) {this->tick = atick;}
	void ticktock();
	void check_tick();
	void zero_tick() {this->tick = 0;}
private:
	double time;
	double tick;
	double default_tick;
	double min_tick;

friend class Network;
};

#endif // CLOCK_H
