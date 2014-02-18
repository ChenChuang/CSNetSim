#ifndef TIMER_H
#define TIMER_H

#include "network.h"

class Timer
{
public:
	Timer(Clock* aclock);
	~Timer();
	void set_after(double after);
	bool is_timeout();
private:
	Clock* clock;
	double time;
};

#endif // TIMER_H
