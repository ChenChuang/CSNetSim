#ifndef ADJV_H
#define ADJV_H

/**
 * element in AdjG
 * */
struct Adjv
{
	int addr;
	double d;
	struct Adjv* next;
	Adjv(int aaddr, int ad, struct Adjv* anext){
		this->addr = aaddr;
		this->d = ad;
		this->next = anext;
	};
	Adjv(){
		this->addr = 0;
		this->d = 0;
		this->next = NULL;
	};
};

#endif