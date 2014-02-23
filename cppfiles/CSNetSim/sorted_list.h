#ifndef SORTEDLIST_H
#define SORTEDLIST_H

#include <cstring>

template<class T>
class SortedListNode
{
public:
	SortedListNode(T* t) : body(t),next(NULL) {}
	~SortedListNode() {delete this->body;}
private:
	T* body;
	SortedListNode<T>* next;

friend class SortedList;	
};

template<class T>
class SortedList
{
public:
	SortedList();
	~SortedList();
public:
	int add(T* t);
	int force_add(T* t);
	int remove(T* t);
	void clear();
	int length();
	T* seek(int i);
	T* tell();
	T* next();

private:
	SortedListNode<T>* head;
	SortedListNode<T>* ptr;
};

#endif // SORTEDLIST_H
