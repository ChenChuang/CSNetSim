#ifndef CHANNELSMANAGER_H
#define CHANNELSMANAGER_H

struct ChannelNode
{
	Channel* body;
	ChannelNode* next;
}

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
}

class ChannelsManager
{
public:
	ChannelsManager(Network* anetwork);
	~ChannelsManager();
public:
	Network* network() {return this->network;}
	void add(Channel* achan);
	void clear();
	ChannelIterator* channel_iter();
	
public:
	Network* network;
	ChannelNode* channels;
	ChannelIterator* channel_iter;
};

#endif // CHANNELSMANAGER_H
