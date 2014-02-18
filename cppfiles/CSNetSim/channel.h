#ifndef CHANNEL_H
#define CHANNEL_H

class Channel
{
public:
	Channel();
	~Channel();
public:
	virtual int communicate(Msg* msg) {return -1;}
};

struct ChannelNode
{
	Channel* channel;
	ChannelNode* next;
}

#endif // CHANNEL_H
