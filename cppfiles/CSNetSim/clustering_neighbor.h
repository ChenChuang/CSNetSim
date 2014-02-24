#ifndef CLUSTERINGNEIGHBOR_H
#define CLUSTERINGNEIGHBOR_H

#include "csnetsim.h"

class Ngb
{
public:
	int addr;
	double d;
public:
	Ngb(int addr, double d) : addr(addr), d(d) {}
	~Ngb() {}
public:
	bool operator <(const Ngb& a) const{
		return d < a.d;
	}
	bool operator ==(const Ngb& a) const{
		return d == a.d;
	}
	bool is(const Ngb& a) const{
		return addr == a.addr;
	} 
};

typedef SortedList<Ngb> NgbManager;

#endif