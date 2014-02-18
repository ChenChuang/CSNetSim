#ifndef CHANNEL_H
#define CHANNEL_H

class Channel
{
public:
	Channel();
	virtual ~Channel();
public:
	virtual int communicate(Msg* msg) {return -1;}
};

class IMonitor_Channel
{
public:
	void record_communicate(Msg* msg, double energy) = 0;
}

#endif // CHANNEL_H
