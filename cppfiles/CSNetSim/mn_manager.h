#ifndef MNMANAGER_H
#define MNMANAGER_H

struct MnNode
{
	int addr;
	struct MnNode* next;
	MnNode(int aaddr): addr(aaddr), next(NULL) {};
};

class MnManager
{
public:
	MnManager(BaseNode* anode);
	~MnManager();
public:
	void remove(int addr);
	void clear();
	void add(int addr);
	MnNode* find(int addr);
	void clear_dead();
	
public:
	BaseNode* node;
	MnNode* members;
};

#endif // MNMANAGER_H
