#ifndef TIMER_H
#define TIMER_H

#include "network.h"

class Timer
{
public:
	Timer(Network* anetwork);
	~Timer();
	void set_after(double after);
	bool is_timeout();
private:
	Network* network;
	double time;
};

#endif // TIMER_H
