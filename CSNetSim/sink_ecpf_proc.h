#ifndef SINKECPFPROC_H
#define SINKECPFPROC_H

#include "csnetsim.h" // Base class: Processor
#include "sensor_ecpf_proc.h"
#include "clustering_comm_proxy.h"

class SinkEcpfProc : public Processor
{
public:
	SinkEcpfProc(Node* node);
	~SinkEcpfProc();
public:
	int process(Msg* msg);
	
private:
	ECommProxy_NetcastChannel* comm;
};

#endif // SINKECPFPROC_H
