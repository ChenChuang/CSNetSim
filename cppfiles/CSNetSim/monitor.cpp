#include "monitor.h"

double Monitor::RECORD_PERIOD;

Monitor::Monitor()
{
	this->record_count = -1;
	this->record_needed = floor(Network::MAX_SIM_TIME / this->RECORD_PERIOD) + 1;
	
	this->rotate_times = 0;
	this->rotate_overhead = 0;
}

Monitor::~Monitor()
{
}

void Monitor::record_AdjG(string file_path, string var_name, AdjG* G)
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

//periodically record the network's conditions
void Monitor::record_periodically(Node** nodes)
{
#ifdef _vc_
	//record the residential energy of WSN in this->energy_sum before main loop ends
	
	//enough records
	if(this->record_count + 1 >= this->record_needed){
		return;
	}
	
	////////////////////////////////////// init recorder, add your recorder's initializtion here ///////////////////////////////
	if(this->record_count == -1){
		//init energy sum recorder
		this->energy_sum = new double[this->record_needed];
		//init alive node sum recorder
		this->alive_sum = new double[this->record_needed];
		//init energy snapshot recorder
		this->energy_snapshot = new double[this->record_needed * Network::NODE_NUM];
		//init node snapshot recorder
		this->node_snapshot = new double[this->record_needed * Network::NODE_NUM];
		//init output recorder
		this->output_track = new double[this->record_needed];
		this->output = 0;
		//init rotate times recorder
		this->rotate_times = 0;
		this->rotate_times_track = new double[this->record_needed];
		//init rotate overhead recorder
		this->rotate_overhead = 0;
		this->rotate_overhead_track = new double[this->record_needed];
		
	}
	
	//check whether it is time to record
	int current_period = floor(Network::sim_timer / this->RECORD_PERIOD) < (this->record_needed - 1) ? floor(Network::sim_timer / this->RECORD_PERIOD) : (this->record_needed - 1);
	
	if(this->record_count == current_period){
		return;
	}
	
	// yes, it is time to record 	
	
	//cal energy sum and alive node sum
	double e_sum = 0;
	int al_sum = 0;
	for(int i = 0; i < Network::NODE_NUM; i ++){
		if(nodes[i]->isAlive){
			e_sum += nodes[i]->energy;
			al_sum ++;
		}
	}
	
	////////////////////////////////// add your record code here ///////////////////////////////////
	for(this->record_count += 1; this->record_count <= current_period; this->record_count ++){
		for(int i = 0; i < Network::NODE_NUM; i ++){
			if(nodes[i]->isAlive){
				this->energy_snapshot[ this->record_count * Network::NODE_NUM + i ] = nodes[i]->energy;
				this->node_snapshot[ this->record_count * Network::NODE_NUM + i ] = nodes[i]->CH_addr;
			}else{
				this->node_snapshot[ this->record_count * Network::NODE_NUM + i ] = nodes[i]->addr;
			}
		}
		this->energy_sum[this->record_count] = e_sum;
		this->alive_sum[this->record_count] = (double)al_sum;
		this->output_track[this->record_count] = this->output;
		this->rotate_overhead_track[this->record_count] = this->rotate_overhead;
		this->rotate_times_track[this->record_count] = (double)(this->rotate_times);
	}
	
	this->record_count --;
	//main loop to continue
	if(this->record_count + 1 < this->record_needed){
		return;
	}
	
	/////////////////////////////// main loop ends, write the result to *.mat //////////////////////////////////
	this->wirte_to_mat("energy_sum.mat","energy_sum", this->energy_sum, 1, this->record_needed);
	this->wirte_to_mat("alive_sum.mat","alive_sum", this->alive_sum, 1, this->record_needed);
	this->wirte_to_mat("energy_snapshot.mat","energy_snapshot", this->energy_snapshot, this->record_needed, Network::NODE_NUM);
	this->wirte_to_mat("node_snapshot.mat","node_snapshot", this->node_snapshot, this->record_needed, Network::NODE_NUM);
	this->wirte_to_mat("output_track.mat","output_track", this->output_track, 1, this->record_needed);
	this->wirte_to_mat("rotate_overhead_track.mat","rotate_overhead_track", this->rotate_overhead_track, 1, this->record_needed);
	this->wirte_to_mat("rotate_times_track.mat","rotate_times_track", this->rotate_times_track, 1, this->record_needed);
	
#endif	
}

void Monitor::record_CH(Node** nodes)
{
#ifdef _vc_
	int node_n = Network::NODE_NUM;

	string file_path = "heed_ch.mat";
	string var_name = "heed_ch";
	mxArray *pa = NULL;
	pa = mxCreateDoubleMatrix(1, node_n, mxREAL);
	MATFile *pmatfile = NULL;
	pmatfile= matOpen(file_path.c_str(),"w");
	
	if(pa == NULL){
		printf("error when allocating mxArray");
		return;
	}
	double *pr = mxGetPr(pa);
#ifdef _print_
	struct MEMBER* p;
#endif
	for(int i = 0; i < node_n; i ++){
		pr[i] = nodes[i]->CH_addr;
#ifdef _print_
		p = nodes[i]->members;
		printf("node %d: ", nodes[i]->addr);
		while(p!=NULL){
			printf("%d, ", p->addr);
			p = p->next;
		}
		printf("\n");
#endif
	}	
	matPutVariable(pmatfile, var_name.c_str(), pa);
	matClose(pmatfile);
	mxDestroyArray(pa);
#endif
}

void Monitor::record_CH_ROUTE(Node** nodes)
{
#ifdef _vc_
	int node_n = Network::NODE_NUM;

	string file_path = "ch_route.mat";
	string var_name = "ch_route";
	mxArray *pa = NULL;
	pa = mxCreateDoubleMatrix(1, node_n, mxREAL);
	MATFile *pmatfile = NULL;
	pmatfile= matOpen(file_path.c_str(),"w");
	
	if(pa == NULL){
		printf("error when allocating mxArray");
		return;
	}
	double *pr = mxGetPr(pa);
	for(int i = 0; i < node_n; i ++){
		if(nodes[i]->CH_addr == nodes[i]->addr){
			pr[i] = nodes[i]->next_hop_addr;
		}else{
			pr[i] = -2;
		}
	}	
	matPutVariable(pmatfile, var_name.c_str(), pa);
	matClose(pmatfile);
	mxDestroyArray(pa);
#endif
}

bool Monitor::wirte_to_mat(string file_path, string var_name, double* array, int row_num, int col_num)
{
	mxArray *pa = NULL;
	pa = mxCreateDoubleMatrix(row_num, col_num, mxREAL);
	MATFile *pmatfile = NULL;
	pmatfile= matOpen(file_path.c_str(),"w");
	
	if(pa == NULL){
		printf("error when allocating mxArray");
		return false;
	}
	
	double *pr = mxGetPr(pa);
	pr = mxGetPr(pa);
	for(int i = 0; i < row_num; i ++){
		for(int j = 0; j < col_num; j ++){
			pr[j * row_num + i] = array[i * col_num + j];
		}
	}
	matPutVariable(pmatfile, var_name.c_str(), pa);
	matClose(pmatfile);
	mxDestroyArray(pa);
	delete array;
	
	return true;
}

