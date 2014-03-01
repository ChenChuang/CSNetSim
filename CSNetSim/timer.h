#ifndef TIMER_H
#define TIMER_H

#include "compile_config.h"
#include "clock.h"

class Timer
{
public:
	Timer(Clock* aclock);
	~Timer();
	void set_after(double after);
	bool is_timeout();
	void reset();
	double get_time() {return this->time;}
private:
	Clock* clock;
	double time;
};

#endif // TIMER_H
