#include "clustering_monitor.h"

ClusteringMonitor::ClusteringMonitor(ClusteringNetwork* anetwork): network(anetwork)
{
	this->min_step = 10;
	this->record_count = 0;
	this->max_records = floor(Network::MAX_SIM_TIME / this->min_step) + 1;
	this->timer = new Timer(this->network->clock());
}

ClusteringMonitor::~ClusteringMonitor()
{
}

ClusteringMonitor::record_before_run()
{
	this->record_adjg("cluster_radius_G.mat", "cluster_radius_G", 
		dynamic_cast<BroadcastChannel*>(this->network->cluster_radius_channel)->adjg);
	this->record_adjg("max_radius_G.mat", "max_radius_G", 
		dynamic_cast<BroadcastChannel*>(this->network->max_radius_channel)->adjg);

	this->time = new double[this->max_records];
	this->energy_sum = new double[this->max_records];
	this->alive_sum = new double[this->max_records];
	this->energy_snapshot = new double[this->max_records * this->network->sensor_nodes_num];
	this->ch_snapshot = new double[this->max_records * this->network->sensor_nodes_num];
	this->hop_snapshot = new double[this->max_records * this->network->sensor_nodes_num];
	this->output = 0;
	this->output_track = new double[this->max_records];
	this->rotate_times = 0;
	this->rotate_times_track = new double[this->max_records];
	this->rotate_overhead = 0;
	this->rotate_overhead_track = new double[this->max_records];
	
	this->timer->set_after(0);
}

ClusteringMonitor::record_in_run()
{
	this->record_periodically(this->network->nodes);
}

ClusteringMonitor::record_after_run()
{
	this->wirte_to_mat("energy_sum.mat","energy_sum", this->energy_sum, 1, this->record_count);
	this->wirte_to_mat("alive_sum.mat","alive_sum", this->alive_sum, 1, this->record_count);
	this->wirte_to_mat("energy_snapshot.mat","energy_snapshot", this->energy_snapshot, this->record_count, this->network->sensor_nodes_num);
	this->wirte_to_mat("ch_snapshot.mat","ch_snapshot", this->ch_snapshot, this->record_count, this->network->sensor_nodes_num);
	this->wirte_to_mat("hop_snapshot.mat","hop_snapshot", this->hop_snapshot, this->record_count, this->network->sensor_nodes_num);
	this->wirte_to_mat("output_track.mat","output_track", this->output_track, 1, this->record_count);
	this->wirte_to_mat("rotate_overhead_track.mat","rotate_overhead_track", this->rotate_overhead_track, 1, this->record_count);
	this->wirte_to_mat("rotate_times_track.mat","rotate_times_track", this->rotate_times_track, 1, this->record_count);
}

ClusteringMonitor::record_communicate(Msg* msg, double energy)
{
	if(msg->cmd != Node::CMD_SENSE_DATA_FUSED && msg->cmd != Node::CMD_SENSE_DATA_UNFUSED){
		this->rotate_overhead += energy;
	}
}

void ClusteringMonitor::record_periodically(Node** nodes)
{
#ifdef _vc_	
	if(!this->timer->is_timeout() || this->record_count >= this->max_records){
		return;
	}
	this->timer->set_after(this->min_step);
	this->time[this->record_count] = this->network->clock()->get_time();
	
	double e_sum = 0;
	int al_sum = 0;
	for(int i = 1; i < this->network->nodes_num; i ++){
		if(nodes[i]->is_alive()){
			e_sum += nodes[i]->energy;
			al_sum ++;
		}
	}	
	for(int i = 1; i < this->network->nodes_num; i ++){
		this->energy_snapshot[ this->record_count * this->network->nodes_num + i ] = nodes[i]->energy;
		this->ch_snapshot[ this->record_count * this->network->nodes_num + i ] = dynamic_cast<SensorNode*>(nodes[i])->ch_addr;
		this->hop_snapshot[ this->record_count * this->network->nodes_num + i ] = dynamic_cast<SensorNode*>(nodes[i])->next_hop;
	}
	this->energy_sum[this->record_count] = e_sum;
	this->alive_sum[this->record_count] = (double)al_sum;
	this->output_track[this->record_count] = this->output;
	this->rotate_overhead_track[this->record_count] = this->rotate_overhead;
	this->rotate_times_track[this->record_count] = (double)(this->rotate_times);
	
	this->record_count++;
#endif	
}

void ClusteringMonitor::record_adjg(string file_path, string var_name, AdjG* G)
{
#ifdef _vc_
	int n = G->v_num;
	//declare the mxArray
	mxArray *pa = NULL;
	//allocate memory for the mxArray, Note:declaration and allocation should be separated, I don't know why.
	pa = mxCreateDoubleMatrix(n, n, mxREAL);
	if(pa == NULL){
		printf("error when allocating mxArray");
		return;
	}
	//declare the MATFile
	MATFile *pmatfile = NULL;
	//use matOpen to create a new *.mat file if it doesn't exist, Note:declaration and creation should be separated, I don't know why too.
	pmatfile= matOpen(file_path.c_str(),"w");
	//use pr to access the mxArray
	double *pr = mxGetPr(pa);
	
	//write data to the normal array pointed by pr
	int i, j, s;
	struct Adjv* p;
	for(i = 0; i < n; i ++){
		s = G->v[i]->addr;
		for(j = 0; j < n; j++){
			pr[j * n + s] = 0;
		}
		p = G->v[i];
		while(p != NULL){
			pr[p->addr * n + s] = 1;
			p = p->next;
		}
	}	
	
	//put mxArray to *.mat, using variable name indicated by the second param
	matPutVariable(pmatfile, var_name.c_str(), pa);
	//close the *.mat
	matClose(pmatfile);
	//release the memory allocated for mxArray
	mxDestroyArray(pa);
#endif
}



