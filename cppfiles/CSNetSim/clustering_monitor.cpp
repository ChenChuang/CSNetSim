#include "clustering_monitor.h"

#include "clustering_network.h"

ClusteringMonitor::ClusteringMonitor(ClusteringNetwork* anetwork): network(anetwork)
{
	this->step = 10;
	this->record_count = 0;
	this->max_records = floor(anetwork->max_time / this->step) + 1;
	this->timer = new Timer(this->network->get_clock());
}

ClusteringMonitor::~ClusteringMonitor()
{
	delete[] this->time;
	delete[] this->energy_sum;
	delete[] this->alive_sum;
	delete[] this->energy_snapshot;
	delete[] this->ch_snapshot;
	delete[] this->hop_snapshot;
	delete[] this->output_track;
	delete[] this->rotate_times_track;
	delete[] this->rotate_overhead_track;
}

void ClusteringMonitor::record_before_run()
{
	//this->record_adjg("cluster_radius_G.mat", "cluster_radius_G", 
	//	dynamic_cast<BroadcastChannel*>(this->network->cluster_radius_channel)->adjg);
	//this->record_adjg("max_radius_G.mat", "max_radius_G", 
	//	dynamic_cast<BroadcastChannel*>(this->network->max_radius_channel)->adjg);

	int i;
	this->time = new double[this->max_records];
	for(i=0;i<this->max_records;i++){
		this->time[i] = -1;
	}
	this->energy_sum = new double[this->max_records];
	for(i=0;i<this->max_records;i++){
		this->energy_sum[i] = -1;
	}
	this->alive_sum = new double[this->max_records];
	for(i=0;i<this->max_records;i++){
		this->alive_sum[i] = -1;
	}
	this->energy_snapshot = new double[this->max_records * this->network->nodes_num];
	for(i=0;i<this->max_records * this->network->nodes_num;i++){
		this->energy_snapshot[i] = -1;
	}
	this->ch_snapshot = new double[this->max_records * this->network->nodes_num];
	for(i=0;i<this->max_records * this->network->nodes_num;i++){
		this->ch_snapshot[i] = -1;
	}
	this->hop_snapshot = new double[this->max_records * this->network->nodes_num];
	for(i=0;i<this->max_records * this->network->nodes_num;i++){
		this->hop_snapshot[i] = -1;
	}
	this->output = 0;
	this->output_track = new double[this->max_records];
	for(i=0;i<this->max_records;i++){
		this->output_track[i] = -1;
	}
	this->rotate_times = 0;
	this->rotate_times_track = new double[this->max_records];
	for(i=0;i<this->max_records;i++){
		this->rotate_times_track[i] = -1;
	}
	this->rotate_overhead = 0;
	this->rotate_overhead_track = new double[this->max_records];
	for(i=0;i<this->max_records;i++){
		this->rotate_overhead_track[i] = -1;
	}
	this->fnd = -1;
	
	this->record_xy(this->network->nodes);
	
	this->timer->set_after(0);
}

void ClusteringMonitor::record_in_run()
{
	this->record_periodically(this->network->nodes);
}

void ClusteringMonitor::record_after_run()
{
	printf("Monitor is writing ... ");
	this->wirte_to_mat("../../../mfiles/time.mat","time", this->time, 1, this->record_count);
	this->wirte_to_mat("../../../mfiles/energy_sum.mat","energy_sum", this->energy_sum, 1, this->record_count);
	this->wirte_to_mat("../../../mfiles/alive_sum.mat","alive_sum", this->alive_sum, 1, this->record_count);
	this->wirte_to_mat("../../../mfiles/energy_snapshot.mat","energy_snapshot", this->energy_snapshot, this->record_count, this->network->nodes_num);
	this->wirte_to_mat("../../../mfiles/ch_snapshot.mat","ch_snapshot", this->ch_snapshot, this->record_count, this->network->nodes_num);
	this->wirte_to_mat("../../../mfiles/hop_snapshot.mat","hop_snapshot", this->hop_snapshot, this->record_count, this->network->nodes_num);
	this->wirte_to_mat("../../../mfiles/output_track.mat","output_track", this->output_track, 1, this->record_count);
	this->wirte_to_mat("../../../mfiles/rotate_overhead_track.mat","rotate_overhead_track", this->rotate_overhead_track, 1, this->record_count);
	this->wirte_to_mat("../../../mfiles/rotate_times_track.mat","rotate_times_track", this->rotate_times_track, 1, this->record_count);
	printf("done\n");
	printf("\nFND = %f\n", this->fnd);
}

void ClusteringMonitor::record_communicate(Msg* msg, double energy)
{
	if(msg->cmd != SensorDataProc::CMD_SENSE_DATA_FUSED && msg->cmd != SensorDataProc::CMD_SENSE_DATA_UNFUSED){
		this->rotate_overhead += energy;
	}
}

void ClusteringMonitor::record_periodically(Node** nodes)
{
	if(!this->timer->is_timeout() || this->record_count >= this->max_records){
		return;
	}
	this->timer->set_after(this->step);
	this->time[this->record_count] = this->network->get_clock()->get_time();
	
	double e_sum = 0;
	int al_sum = 0;
	for(int i = 1; i < this->network->nodes_num; i ++){
		if(nodes[i]->is_alive()){
			e_sum += nodes[i]->energy;
			al_sum ++;
		}else if(this->fnd < 0){
			this->fnd = this->network->get_clock()->get_time();
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
}

void ClusteringMonitor::record_xy(Node** nodes)
{
	double* xs = new double[this->network->nodes_num];
	double* ys = new double[this->network->nodes_num];
	for(int i=0; i<this->network->nodes_num; i++){
		xs[i] = nodes[i]->x;
		ys[i] = nodes[i]->y;
	}
	this->wirte_to_mat("../../../mfiles/nodes_x.mat","nodes_x", xs, 1, this->network->nodes_num);
	this->wirte_to_mat("../../../mfiles/nodes_y.mat","nodes_y", ys, 1, this->network->nodes_num);
}

/** deprecated **/
void ClusteringMonitor::record_adjg(std::string file_path, std::string var_name, AdjG* G)
{
	int n = this->network->sensor_nodes_num;
	double* pr = new double[n*n];
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
	delete[] pr;
}