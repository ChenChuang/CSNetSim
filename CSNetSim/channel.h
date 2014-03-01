#ifndef CHANNEL_H
#define CHANNEL_H

#include "compile_config.h"
#include "msg.h"

class Network;

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
	virtual void record_communicate(Msg* msg, double energy) = 0;
};

struct ChannelNode
{
	Channel* body;
	ChannelNode* next;
	ChannelNode(Channel* achannel): body(achannel), next(NULL) {}
};

class ChannelIterator
{
public:
	ChannelIterator(ChannelNode* head): ptr(head) {};
	~ChannelIterator() {};
public:
	Channel* next()
	{
		Channel* chan = NULL;
		if(this->ptr != NULL){
			chan = this->ptr->body;
			this->ptr = this->ptr->next;
		}
		return chan;
	}
	bool has_more() {return this->ptr != NULL;}
	void seek(ChannelNode* head) {this->ptr = head;}

private:
	ChannelNode* ptr;
};

class ChannelsManager
{
public:
	ChannelsManager(Network* anetwork);
	~ChannelsManager();
public:
	Network* get_network() {return this->network;}
	void add(Channel* achan);
	void clear();
	ChannelIterator* get_channel_iter();
	
public:
	Network* network;
	ChannelNode* channels;
	ChannelIterator* channel_iter;
};

#endif // CHANNEL_H
