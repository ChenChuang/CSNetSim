#ifndef ADJG_H
#define ADJG_H

#include "Adjv.h"
#include <math.h>
#include <stdio.h>

/**
 * AdjG of WSN under certain connect-radius
 * */
class AdjG
{
public:
	//create AdjG using x, y coordiate array and r as connect-radius of node. n is the number of nodes
	AdjG(double* x, double* y, double r, int n);
	AdjG(Network* anetwork, double r, int n);
	~AdjG();
public:
	//print the AdjG as link-list array
	void print();
	//delete the node with addr in AdjG when it dies
	void delete_node(int addr);
	Adjv* neighbors_of(int addr);
	bool is_neighbor(int addr_a, int addr_b);
	
public:
	double radius;    //connect-radius of node
	int v_num;          //number of nodes
	struct Adjv** v;    //pointer of link-list array
};

#endif // ADJG_H
