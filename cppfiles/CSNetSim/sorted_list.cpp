#include "sorted_list.h"

template<class T>
SortedList<T>::SortedList() : head(NULL), ptr(NULL)
{
}

template<class T>
SortedList<T>::~SortedList()
{
	this->clear();
}

template<class T>
int SortedList<T>::add(T* t)
{
	SortedListNode<T>* p = this->head;
	SortedListNode<T>* pp = NULL;
	while(p != NULL){
		if(t == p->body){
			return -1;
		}
		if(*t == *(p->body)){
			return 0;
		}
		if(*t < *(p->body)){
			break;
		}
		pp = p;
		p = p->next;
	}
	SortedListNode<T>* pt = new SortedListNode<T>(t);
	if(pp == NULL){
		if(p == NULL){
			this->head = pt;
			this->ptr = this->head;
		}else{
			this->head = pt;
			pt->next = p;
		}
	}else{
		p->next = pp->next;
		pp->next = p;
	}
	return 1;
}

template<class T>
int SortedList<T>::force_add(T* t)
{
	SortedListNode<T>* p = this->head;
	SortedListNode<T>* pp = NULL;
	while(p != NULL){
		if(t == p->body){
			return -1;
		}
		if(*t <= *(p->body)){
			break;
		}
		pp = p;
		p = p->next;
	}
	SortedListNode<T>* pt = new SortedListNode<T>(t);
	if(pp == NULL){
		if(p == NULL){
			this->head = pt;
			this->ptr = this->head;
		}else{
			this->head = pt;
			pt->next = p;
		}
	}else{
		p->next = pp->next;
		pp->next = p;
	}
	return 1;
}

template<class T>
T* SortedList<T>::next()
{
	if(this->ptr == NULL){
		return NULL;
	}else{
		return this->ptr->next->body;
	}
}

template<class T>
int SortedList<T>::remove(T* t)
{
	SortedListNode<T>* p = this->head;
	SortedListNode<T>* pp = NULL;
	while(p != NULL){
		if(t == p->body){
			break;
		}
		pp = p;
		p = p->next;
	}
	if(pp == NULL){
		if(p == NULL){
			return 0;
		}else{
			delete p;
			this->head = NULL;
			this->ptr = NULL;
			return 1;
		}
	}else if(p != NULL){
		pp->next = p->next;
		delete p;
		return 1;
	}
	return 0;
}

template<class T>
T* SortedList<T>::seek(int i)
{
	if(i < 0){
		i = this->length() + i;
	}
	int ii = 0;
	SortedListNode<T>* p = this->head;
	while(p != NULL && ii < i){
		p = p->next;
	}
	this->ptr = p;
	return p;
}

template<class T>
void SortedList<T>::clear()
{
	SortedListNode<T>* p = this->head;
	SortedListNode<T>* np;
	while(p != NULL){
		np = p->next;
		delete p;
		p = np;
	}
	this->head = NULL;
	this->ptr = NULL;
}

template<class T>
T* SortedList<T>::tell()
{
	return this->ptr->body;
}

template<class T>
int SortedList<T>::length()
{
	int l = 0;
	SortedListNode<T>* p = this->head;
	while(p != NULL){
		l++;
		p = p->next;
	}
	return l;
}
