#include <cstring>

template<class T>
class SortedList;

template<class T>
class SortedListNode
{
public:
	SortedListNode(T* t) : body(t),next(NULL) {}
	~SortedListNode() {delete this->body;}
public:
	T* body;
	SortedListNode<T>* next;

friend class SortedList<T>;	
};

template<class T>
class SortedList
{
public:
	SortedList();
	~SortedList();
public:
	int add(T* t);
	int remove_equalto(const T& t);
	int remove(const T& t);
	T* find(const T& t);
	void clear();
	int length();
	bool has_more();
	T* seek(int i);
	T* tell();
	T* next();

public:
	SortedListNode<T>* head;
	SortedListNode<T>* ptr;
};

template<class T>
class SortedListIter
{
public:
	SortedListIter(SortedList<T>* s) : head(s->head), ptr(s->head) {}
	~SortedListIter() {}
	void reset() {this->ptr = this->head;}
	bool has_more() {return this->ptr != NULL;}
	T* next() {
		if(this->ptr == NULL){
			return NULL;
		}
		T* t = this->ptr->body;
		this->ptr = this->ptr->next;
		return t;
	}
public:
	SortedListNode<T>* head;
	SortedListNode<T>* ptr;
};

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
		if(p->body->is(*t)){
			return -1;
		}
		if(*t < *(p->body) || *t == *(p->body)){
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
		pt->next = pp->next;
		pp->next = pt;
	}
	return 1;
}

template<class T>
T* SortedList<T>::next()
{
	if(this->ptr == NULL){
		return NULL;
	}else{
		T* t = this->ptr->body;
		this->ptr = this->ptr->next;
		return t;
	}
}

template<class T>
int SortedList<T>::remove_equalto(const T& t)
{
	SortedListNode<T>* p = this->head;
	SortedListNode<T>* pp = NULL;
	while(p != NULL){
		if(t == *(p->body)){
			if(pp == NULL){
				this->head = p->next;
				delete p;
				p = this->head;
				pp = NULL;
			}else{
				pp->next = p->next;
				delete p;
				p = pp->next;
			}
		}else{
			pp = p;
			p = p->next;
		}
	}
	if(this->head == NULL){
		this->ptr = NULL;
	}
	if(p == NULL){
		return 0;
	}else{
		return 1;
	}
}

template<class T>
int SortedList<T>::remove(const T& t)
{
	SortedListNode<T>* p = this->head;
	SortedListNode<T>* pp = NULL;
	while(p != NULL){
		if(p->body->is(t)){
			if(pp == NULL){
				this->head = p->next;
				delete p;
				p = this->head;
				pp = NULL;
			}else{
				pp->next = p->next;
				delete p;
				p = pp->next;
			}
		}else{
			pp = p;
			p = p->next;
		}
	}
	if(this->head == NULL){
		this->ptr = NULL;
	}
	if(p == NULL){
		return 0;
	}else{
		return 1;
	}
}

template<class T>
T* SortedList<T>::find(const T& t)
{
	SortedListNode<T>* p = this->head;
	while(p != NULL){
		if(p->body->is(t)){
			return p->body;
		}
		p = p->next;
	}
	return NULL;
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
		ii++;
	}
	this->ptr = p;
	if(p != NULL){
		return this->ptr->body;
	}else{
		return NULL;
	}
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

template<class T>
bool SortedList<T>::has_more()
{
	return this->ptr != NULL;
}

