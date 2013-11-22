#ifndef ADJV_H
#define ADJV_H

/**
 * element in AdjG
 * */
struct adjv
{
	int addr;
	double d;
	struct adjv* next;
};

#endif