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

#endif // CHANNEL_H
