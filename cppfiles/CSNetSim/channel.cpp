#include "channel.h"

Channel::Channel()
{
}

Channel::~Channel()
{
}

ChannelsManager::ChannelsManager(Network* anetwork): network(anetwork), channels(NULL)
{
	this->channel_iter = new ChannelIterator(this->channels);
}

ChannelsManager::~ChannelsManager()
{
	delete this->channel_iter;
	this->channel_iter = NULL;
	this->clear();
}

void ChannelsManager::add(Channel* channel)
{
	ChannelNode* p = this->channels;
	ChannelNode* pp = NULL;
	while(p != NULL){
		pp = p;
		p = p->next;
	}
	p = new ChannelNode(channel);
	if(pp != NULL){
		pp->next = p;
	}else{
		this->channels = p;
	}
}

void ChannelsManager::clear()
{
	ChannelNode* p = this->channels;
	ChannelNode* np;
	while(p != NULL){
		np = p->next;
		delete p;
		p = np;
	}
}

ChannelIterator* ChannelsManager::get_channel_iter()
{
	this->channel_iter->seek(this->channels);
	return this->channel_iter;
}