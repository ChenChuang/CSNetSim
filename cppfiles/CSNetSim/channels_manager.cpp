#include "channels_manager.h"

ChannelsManager::ChannelsManager(Network* anetwork): network(anetwork), channels(NULL)
{
	this->channel_iter = new ChannelIterator(this->channels);
}

ChannelsManager::~ChannelsManager()
{
	delete this->channel_iter;
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
		delete np;
		p = np;
	}
}

ChannelIterator* ChannelsManager::channel_iter()
{
	this->channel_iter->seek(this->channels);
	return this->channel_iter;
}