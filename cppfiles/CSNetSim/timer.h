#ifndef TIMER_H
#define TIMER_H

#include "base_network.h"

class Timer
{
public:
	Timer(BaseNetwork* anetwork);
	~Timer();
	void set_after(double after);
	bool is_timeout();
private:
	BaseNetwork* network;
	double time;
};

#endif // TIMER_H
