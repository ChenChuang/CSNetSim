#include "clustering_monitor.h"

#include "clustering_network.h"

ClusteringMonitor::ClusteringMonitor(ClusteringNetwork* anetwork): network(anetwork)
{
	this->step = ClusteringSimModel::RECORD_PERIOD;
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
	
	this->output = dynamic_cast<SinkNode*>(nodes[0])->dataproc->data_l;
	
	double e_sum = 0;
	int al_sum = 0;
	for(int i = 1; i < this->network->nodes_num; i++){
		if(nodes[i]->is_alive()){
			e_sum += nodes[i]->energy;
			al_sum ++;
		}else if(this->fnd < 0){
			this->fnd = this->network->get_clock()->get_time();
		}
	}
	if(this->lnd < 0 && al_sum == 0){
		this->lnd = this->network->get_clock()->get_time();
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

void ClusteringMonitor::record_rotate(int a, int na)
{
	std::ofstream f("../../results/lcr/rotate.dat", std::ios::binary|std::ios::app);
	double* array = new double[7];
	array[0] = (double)a;
	array[1] = this->network->nodes[a]->x;
	array[2] = this->network->nodes[a]->y;
	array[3] = (double)na;
	array[4] = this->network->nodes[na]->x;
	array[5] = this->network->nodes[na]->y;
	array[6] = this->network->get_clock()->get_time();
	f.write((char*)array, 7*sizeof(double));
	f.close();
	delete[] array;
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
	this->lnd = -1;
#ifdef _LCR_
	std::remove("../../results/lcr/rotate.dat");
#endif
	this->timer->set_after(0);
}

void ClusteringMonitor::record_in_run()
{
	this->record_periodically(this->network->nodes);
}

void ClusteringMonitor::record_after_run()
{
	double* xs = new double[this->network->nodes_num];
	double* ys = new double[this->network->nodes_num];
	for(int i=0; i<this->network->nodes_num; i++){
		xs[i] = this->network->nodes[i]->x;
		ys[i] = this->network->nodes[i]->y;
	}
	printf("Monitor is writing ... ");
#ifdef _WRITE_DAT_
#ifdef _HEED_
	this->write_to_dat("../../results/heed/nodes_x.dat", xs, this->network->nodes_num);
	this->write_to_dat("../../results/heed/nodes_y.dat", ys, this->network->nodes_num);
	this->write_to_dat("../../results/heed/time.dat", this->time, this->record_count);
	this->write_to_dat("../../results/heed/energy_sum.dat", this->energy_sum, this->record_count);
	this->write_to_dat("../../results/heed/alive_sum.dat", this->alive_sum, this->record_count);
	this->write_to_dat("../../results/heed/energy_snapshot.dat", this->energy_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/heed/ch_snapshot.dat", this->ch_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/heed/hop_snapshot.dat", this->hop_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/heed/output_track.dat", this->output_track, this->record_count);
	this->write_to_dat("../../results/heed/rotate_overhead_track.dat", this->rotate_overhead_track, this->record_count);
	this->write_to_dat("../../results/heed/rotate_times_track.dat", this->rotate_times_track, this->record_count);
#endif
#ifdef _LCR_
	this->write_to_dat("../../results/lcr/nodes_x.dat", xs, this->network->nodes_num);
	this->write_to_dat("../../results/lcr/nodes_y.dat", ys, this->network->nodes_num);
	this->write_to_dat("../../results/lcr/time.dat", this->time, this->record_count);
	this->write_to_dat("../../results/lcr/energy_sum.dat", this->energy_sum, this->record_count);
	this->write_to_dat("../../results/lcr/alive_sum.dat", this->alive_sum, this->record_count);
	this->write_to_dat("../../results/lcr/energy_snapshot.dat", this->energy_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/lcr/ch_snapshot.dat", this->ch_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/lcr/hop_snapshot.dat", this->hop_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/lcr/output_track.dat", this->output_track, this->record_count);
	this->write_to_dat("../../results/lcr/rotate_overhead_track.dat", this->rotate_overhead_track, this->record_count);
	this->write_to_dat("../../results/lcr/rotate_times_track.dat", this->rotate_times_track, this->record_count);
#endif
#ifdef _ECPF_
	this->write_to_dat("../../results/ecpf/nodes_x.dat", xs, this->network->nodes_num);
	this->write_to_dat("../../results/ecpf/nodes_y.dat", ys, this->network->nodes_num);
	this->write_to_dat("../../results/ecpf/time.dat", this->time, this->record_count);
	this->write_to_dat("../../results/ecpf/energy_sum.dat", this->energy_sum, this->record_count);
	this->write_to_dat("../../results/ecpf/alive_sum.dat", this->alive_sum, this->record_count);
	this->write_to_dat("../../results/ecpf/energy_snapshot.dat", this->energy_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/ecpf/ch_snapshot.dat", this->ch_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/ecpf/hop_snapshot.dat", this->hop_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/ecpf/output_track.dat", this->output_track, this->record_count);
	this->write_to_dat("../../results/ecpf/rotate_overhead_track.dat", this->rotate_overhead_track, this->record_count);
	this->write_to_dat("../../results/ecpf/rotate_times_track.dat", this->rotate_times_track, this->record_count);
#endif
#ifdef _IFUCM_
	this->write_to_dat("../../results/ifucm/nodes_x.dat", xs, this->network->nodes_num);
	this->write_to_dat("../../results/ifucm/nodes_y.dat", ys, this->network->nodes_num);
	this->write_to_dat("../../results/ifucm/time.dat", this->time, this->record_count);
	this->write_to_dat("../../results/ifucm/energy_sum.dat", this->energy_sum, this->record_count);
	this->write_to_dat("../../results/ifucm/alive_sum.dat", this->alive_sum, this->record_count);
	this->write_to_dat("../../results/ifucm/energy_snapshot.dat", this->energy_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/ifucm/ch_snapshot.dat", this->ch_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/ifucm/hop_snapshot.dat", this->hop_snapshot, this->record_count*this->network->nodes_num);
	this->write_to_dat("../../results/ifucm/output_track.dat", this->output_track, this->record_count);
	this->write_to_dat("../../results/ifucm/rotate_overhead_track.dat", this->rotate_overhead_track, this->record_count);
	this->write_to_dat("../../results/ifucm/rotate_times_track.dat", this->rotate_times_track, this->record_count);
#endif
#endif
	printf("done\n");
	printf("\nFND = %f\nLND = %f\nOutput = %f\n", this->fnd, this->lnd, this->output);
	printf("\nsizeof(double) = %d\n", sizeof(double));
	printf("record_num = %d\n", this->record_count);
	printf("node_num = %d\n", this->network->nodes_num);
}