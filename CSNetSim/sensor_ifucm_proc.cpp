#include "sensor_ifucm_proc.h"

SensorIfucmProc::SensorIfucmProc(Node* anode) : node(anode)
{
	this->inode = dynamic_cast<INode_SensorIfucmProc*>(this->node);
    this->inetwork = dynamic_cast<INet_SensorIfucmProc*>(this->node->get_network());
    this->comm = dynamic_cast<ClusteringCommProxy*>(this->node->get_commproxy());
	this->min_tick = 0.01;
	this->tent_p = 0.1;
	this->ifucm_time = 1;
	this->route_time = 1;
	this->stable_time = ClusteringSimModel::SENSE_DATA_PERIOD * 65;
	this->max_ifucm_count = 10;
	
	this->tents = new SortedList<ifucm::Tent>();
    this->chs = new SortedList<ifucm::Sch>();
	this->timer = new Timer(this->node->get_network()->get_clock());
}

SensorIfucmProc::~SensorIfucmProc()
{
	delete this->tents;
	this->tents = NULL;
    delete this->chs;
	this->chs = NULL;
	delete this->timer;
	this->timer = NULL;
}

void SensorIfucmProc::init()
{
	this->proc_state = SensorIfucmProc::PROC_SLEEP;
}

double SensorIfucmProc::start_clustering()
{
	this->inode->set_ch_addr(-1);
	this->inode->set_next_hop(-1);
	this->proc_state = SensorIfucmProc::PROC_GETREADY;
	this->timer->set_after(this->ifucm_time);
	return this->ifucm_time;
}

void SensorIfucmProc::exit_clustering()
{
	if(this->inode->get_ch_addr() < 0){
		this->inode->set_ch_addr(this->node->get_addr());
	}
	this->proc_state = SensorIfucmProc::PROC_SLEEP;
	this->timer->set_after(this->route_time + this->stable_time);
	this->inode->start_route(this->radius * 2);
}

void SensorIfucmProc::ticktock(double time)
{
	if(this->timer->is_timeout()){
		if(this->proc_state == SensorIfucmProc::PROC_SLEEP){
			this->start_clustering();
			this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		}else{
			this->exit_clustering();
		}
	}else{
		if(this->proc_state == SensorIfucmProc::PROC_SLEEP){
			this->node->get_network()->get_clock()->try_set_tick(
				this->timer->get_time() - this->node->get_network()->get_clock()->get_time());
		}else{
			this->proc_clustering();
		}
	}
}

int SensorIfucmProc::process(Msg* msg)
{
	switch(msg->cmd)
	{
	case SensorIfucmProc::CMD_COMPETE:
	{
		this->receive_compete_msg(msg);
		return 1;
	}
	case SensorIfucmProc::CMD_QUIT:
	{
		this->receive_quit_msg(msg);
		return 1;
	}
    case SensorIfucmProc::CMD_CH:
    {
        this->receive_ch_msg(msg);
        return 1;
    }
	case SensorIfucmProc::CMD_JOIN:
	{
		this->receive_join_msg(msg);
		return 1;
	}
	}
	return -1;
}

int SensorIfucmProc::proc_clustering()
{	
	switch(this->proc_state)
	{
	case SensorIfucmProc::PROC_SLEEP:
	{
		break;
	}
	case SensorIfucmProc::PROC_DONE:
	{
		break;
	}
	case SensorIfucmProc::PROC_GETREADY:
	{
		this->tents->clear();
        this->chs->clear();
	    this->cost = -1;
        this->radius = -1;
        this->is_final = false;
		this->ifucm_count = 0;
	
        if((double)rand() / (RAND_MAX+1.0) < this->tent_p){    
		    this->proc_state = SensorIfucmProc::PROC_COMPETE;
            this->cal_radius_cost();
            this->add_tent(this->node->get_addr(), this->cost);
            this->broadcast_compete_msg(); 
        }else{
		    this->proc_state = SensorIfucmProc::PROC_NOTCOMPETE;
        }
		this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		break;
	}
	case SensorIfucmProc::PROC_COMPETE:
	{
		if(this->ifucm_count < this->max_ifucm_count-1){
			bool is_receive_final = false;
			this->chs->seek(0);
			ifucm::Sch* sc = NULL;
			while(this->chs->has_more()){
				sc = this->chs->next();
				if(this->tents->find(ifucm::Tent(sc->addr, 0)) != NULL){
					is_receive_final = true;
					break;
				}
			}
			if(is_receive_final){
				this->broadcast_quit_msg();
				this->is_final = false;
				this->proc_state = SensorIfucmProc::PROC_NOTCOMPETE;
			}
			int best = this->get_least_cost_tent();
			if(best >= 0 && this->node->get_addr() == best){
				this->broadcast_ch_msg();
				this->is_final = true;
				this->proc_state = SensorIfucmProc::PROC_NOTCOMPETE;
			}
			this->ifucm_count++;
		}else if(this->ifucm_count < this->max_ifucm_count){
			this->broadcast_ch_msg();
			this->is_final = true;
			this->proc_state = SensorIfucmProc::PROC_NOTCOMPETE;
		}else{
			this->proc_state = SensorIfucmProc::PROC_FINAL;
		}
		this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		break;
	}
	case SensorIfucmProc::PROC_NOTCOMPETE:
	{
		if(this->ifucm_count < this->max_ifucm_count){
			this->ifucm_count++;
		}else{
			this->proc_state = SensorIfucmProc::PROC_FINAL;
		}
		this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
        break;
    }
	case SensorIfucmProc::PROC_FINAL:
	{
        int best;
	    if(this->is_final){
            best = this->node->get_addr();
        }else{
            best = this->get_closest_ch();
            if(best < 0){
                best = this->node->get_addr();
            }else{
                this->unicast_join_msg(best);
            }
        }
        this->inode->set_ch_addr(best);
        this->proc_state = SensorIfucmProc::PROC_DONE;
		this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		break;
	}
	}
	return 1;
}

void SensorIfucmProc::broadcast_ch_msg()
{
	this->comm->broadcast(
		this->node->get_addr(), ClusteringSimModel::CLUSTER_RADIUS, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorIfucmProc::CMD_CH, 
		0, NULL);
}

void SensorIfucmProc::receive_ch_msg(Msg* msg)
{
    this->add_ch(msg->fromaddr);
}

void SensorIfucmProc::receive_compete_msg(Msg* msg)
{
	this->add_tent(msg->fromaddr, ((double*)(msg->data))[0]);
}

void SensorIfucmProc::receive_join_msg(Msg* msg)
{
	this->inode->set_ch_addr(this->node->get_addr());
	this->add_member(msg->fromaddr);
}

void SensorIfucmProc::broadcast_quit_msg()
{
	this->comm->broadcast(
		this->node->get_addr(), ClusteringSimModel::CLUSTER_RADIUS, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorIfucmProc::CMD_QUIT, 
		0, NULL);
}

void SensorIfucmProc::receive_quit_msg(Msg* msg)
{
	this->tents->remove(ifucm::Tent(msg->fromaddr, 0));
	this->chs->remove(ifucm::Sch(msg->fromaddr, 0));
}

void SensorIfucmProc::add_ch(int addr)
{
    double d = this->inetwork->d_between(this->node->get_addr(), addr);
    this->chs->add(new ifucm::Sch(addr, d));
}

void SensorIfucmProc::add_member(int addr)
{
	this->inode->get_mnmanager()->add(addr);
}

void SensorIfucmProc::add_tent(int addr, double cost)
{
	this->tents->add(new ifucm::Tent(addr, cost));
}

void SensorIfucmProc::broadcast_compete_msg()
{
	double* data = new double[1];
	data[0] = this->cost;
	this->add_tent(this->node->get_addr(), this->cost);
	this->comm->broadcast(
		this->node->get_addr(), this->radius, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorIfucmProc::CMD_COMPETE, 
		sizeof(double), (char*)data);
	delete[] data;
}

int SensorIfucmProc::get_least_cost_tent()
{
	ifucm::Tent* sc = NULL;
	this->tents->seek(0);
	while(this->tents->has_more()){
		sc = this->tents->next();
		return sc->addr;
	}
	return -1;
}

int SensorIfucmProc::get_closest_ch()
{
	ifucm::Sch* sc = NULL;
	this->chs->seek(0);
	while(this->chs->has_more()){
		sc = this->chs->next();
		return sc->addr;
	}
	return -1;
}

void SensorIfucmProc::unicast_join_msg(int addr)
{
	this->comm->unicast(
		this->node->get_addr(), addr, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorIfucmProc::CMD_JOIN, 
		0, NULL);
}

void SensorIfucmProc::cal_radius_cost()
{
    double chance = 0;
    double radius = 0;
    double dens = 0;
	NgbManager* ngbs = this->inode->get_neighbors();
	Ngb* ngb;
	ngbs->seek(0);
	while(ngbs->has_more()){
		ngb = ngbs->next();
		if(ngb->d <= ClusteringSimModel::CLUSTER_RADIUS){
			dens++;
		}
	}
    ifucm::fcc->cal(this->node->energy, this->inode->get_d_tosink(), dens, chance, radius);
	this->cost = 1.0 - chance;
	this->radius = radius;
}

ifucm::FuzzyCostComputor::FuzzyCostComputor()
{
	this->engine = new fl::Engine;
	this->engine->setName("FuzzyCostComputor");
	
	this->energy = new fl::InputVariable;
	this->energy->setEnabled(true);
	this->energy->setName("ener");
	this->energy->setRange(0.000, ClusteringSimModel::E_INIT+1.0);

    double le = 0.100 * ClusteringSimModel::E_INIT;
    double ae = 0.500 * ClusteringSimModel::E_INIT;
    double me = 0.900 * ClusteringSimModel::E_INIT;
	this->energy->addTerm(new fl::Trapezoid("l", 0.000, 0.000, le, ae));
	this->energy->addTerm(new fl::Triangle("m", le, ae, me));
	this->energy->addTerm(new fl::Trapezoid("h", ae, me, ClusteringSimModel::E_INIT+1.0, ClusteringSimModel::E_INIT+1.0));
	this->engine->addInputVariable(this->energy);
	
	this->dist = new fl::InputVariable;
	this->dist->setEnabled(true);
	this->dist->setName("dist");
    
    double maxd = sqrt(pow(ClusteringSimModel::AREA_SIZE_X,2) + pow(ClusteringSimModel::AREA_SIZE_Y,2))+1.0;
	this->dist->setRange(0.000, maxd);

	double ld = 0.2000 * maxd;
	double ad = 0.5000 * maxd;
	double md = 0.8000 * maxd;
	this->dist->addTerm(new fl::Trapezoid("s", 0.000, 0.000, ld, ad));
	this->dist->addTerm(new fl::Triangle("m", ld, ad, md));
	this->dist->addTerm(new fl::Trapezoid("f", ad, md, maxd, maxd));
	this->engine->addInputVariable(this->dist);
	
    this->dens = new fl::InputVariable;
	this->dens->setEnabled(true);
	this->dens->setName("dens");
 
    double an = ClusteringSimModel::NODE_NUM / 
		(ClusteringSimModel::AREA_SIZE_X * ClusteringSimModel::AREA_SIZE_Y / pow(ClusteringSimModel::CLUSTER_RADIUS, 2) / 3.14);
	double mn = an*2;
	this->dens->setRange(0.000, ClusteringSimModel::NODE_NUM+1.0);

	this->dens->addTerm(new fl::Trapezoid("l", 0.000, 0.000, mn/12, mn/3));
	this->dens->addTerm(new fl::Triangle("m", mn/12, mn*5/12, mn*5/6));
	this->dens->addTerm(new fl::Trapezoid("h", mn/2, mn*5/6, ClusteringSimModel::NODE_NUM+1.0, ClusteringSimModel::NODE_NUM+1.0));
	this->engine->addInputVariable(this->dens);
	
	this->chan = new fl::OutputVariable;
	this->chan->setName("chan");
	this->chan->setRange(0.000, 1.000);
	this->chan->fuzzyOutput()->setAccumulation(new fl::Maximum);
	this->chan->setDefuzzifier(new fl::Centroid(100));
	this->chan->setDefaultValue(fl::nan);
	this->chan->setLockValidOutput(false);
	this->chan->setLockOutputRange(false);

	this->chan->addTerm(new fl::Trapezoid("ll", 0.0, 0.0, 0.1, 0.3));
	this->chan->addTerm(new fl::Triangle("ml", 0.1, 0.3, 0.5));
	this->chan->addTerm(new fl::Triangle("mm", 0.3, 0.5, 0.7));
	this->chan->addTerm(new fl::Triangle("mh", 0.5, 0.7, 0.9));
	this->chan->addTerm(new fl::Trapezoid("hh", 0.7, 0.9, 1.0, 1.0));
	this->engine->addOutputVariable(this->chan);
	
    this->rad = new fl::OutputVariable;
	this->rad->setName("rad");
	this->rad->setRange(0.000, 1.0);
	this->rad->fuzzyOutput()->setAccumulation(new fl::Maximum);
	this->rad->setDefuzzifier(new fl::Centroid(100));
	this->rad->setDefaultValue(fl::nan);
	this->rad->setLockValidOutput(false);
	this->rad->setLockOutputRange(false);

	this->rad->addTerm(new fl::Trapezoid("ss", 0.0, 0.0, 0.1, 0.3));
	this->rad->addTerm(new fl::Triangle("ms", 0.1, 0.3, 0.5));
	this->rad->addTerm(new fl::Triangle("mm", 0.3, 0.5, 0.7));
	this->rad->addTerm(new fl::Triangle("mf", 0.5, 0.7, 0.9));
	this->rad->addTerm(new fl::Trapezoid("ff", 0.7, 0.9, 1.0, 1.0));
	this->engine->addOutputVariable(this->rad);

	this->rules = new fl::RuleBlock;
	this->rules->setEnabled(true);
	this->rules->setName("");
	this->rules->setConjunction(NULL);
	this->rules->setDisjunction(NULL);
	this->rules->setActivation(new fl::Minimum);

	this->rules->addRule(fl::Rule::parse("if ener is h and dist is f and dens is h then chan is hh and rad is mf", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is h and dist is f and dens is m then chan is mh and rad is ff", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is h and dist is f and dens is l then chan is mm and rad is ff", this->engine));

	this->rules->addRule(fl::Rule::parse("if ener is h and dist is m and dens is h then chan is hh and rad is mf", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is h and dist is m and dens is m then chan is mm and rad is mm", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is h and dist is m and dens is l then chan is ml and rad is mm", this->engine));

	this->rules->addRule(fl::Rule::parse("if ener is h and dist is s and dens is h then chan is hh and rad is ms", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is h and dist is s and dens is m then chan is mh and rad is ss", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is h and dist is s and dens is l then chan is mm and rad is ss", this->engine));


    this->rules->addRule(fl::Rule::parse("if ener is m and dist is f and dens is h then chan is mh and rad is mf", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is m and dist is f and dens is m then chan is mm and rad is ff", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is m and dist is f and dens is l then chan is ml and rad is ff", this->engine));

	this->rules->addRule(fl::Rule::parse("if ener is m and dist is m and dens is h then chan is mh and rad is mf", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is m and dist is m and dens is m then chan is mm and rad is mm", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is m and dist is m and dens is l then chan is ml and rad is mm", this->engine));

	this->rules->addRule(fl::Rule::parse("if ener is m and dist is s and dens is h then chan is ml and rad is ms", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is m and dist is s and dens is m then chan is ml and rad is ss", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is m and dist is s and dens is l then chan is ll and rad is ss", this->engine));


    this->rules->addRule(fl::Rule::parse("if ener is l and dist is f and dens is h then chan is mm and rad is mf", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is l and dist is f and dens is m then chan is mm and rad is ff", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is l and dist is f and dens is l then chan is ml and rad is ff", this->engine));

	this->rules->addRule(fl::Rule::parse("if ener is l and dist is m and dens is h then chan is ml and rad is mf", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is l and dist is m and dens is m then chan is ml and rad is mm", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is l and dist is m and dens is l then chan is ml and rad is mm", this->engine));

	this->rules->addRule(fl::Rule::parse("if ener is l and dist is s and dens is h then chan is ml and rad is ms", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is l and dist is s and dens is m then chan is ml and rad is ss", this->engine));
	this->rules->addRule(fl::Rule::parse("if ener is l and dist is s and dens is l then chan is ml and rad is ss", this->engine));

	this->engine->addRuleBlock(this->rules);	this->engine->addRuleBlock(this->rules);
	
	this->engine->configure();
	
	std::string status;
	if (not engine->isReady(&status)){
		throw fl::Exception("Engine not ready. "
            "The following errors were encountered:\n" + status, FL_AT);
	}
}

ifucm::FuzzyCostComputor::~FuzzyCostComputor()
{
	delete this->engine;
	this->engine = NULL;
	delete this->energy;
	this->energy = NULL;
	delete this->dist;
	this->dist = NULL;
    delete this->dens;
	this->dens = NULL;
	delete this->chan;
	this->chan = NULL;
    delete this->rad;
    this->rad = NULL;
	delete this->rules;
	this->rules = NULL;
}

void ifucm::FuzzyCostComputor::cal(double aenergy, double adist, double adens, double& chance, double& radius)
{ 
	fl::scalar energy = aenergy;
	fl::scalar dist = adist;
    fl::scalar dens = adens;
	this->energy->setInputValue(energy);
	this->dist->setInputValue(dist);
    this->dens->setInputValue(dens);
	this->engine->process();
	chance = this->chan->defuzzify();
    double rp = this->rad->defuzzify();
	double k = 0.7;
	radius = ClusteringSimModel::CLUSTER_RADIUS * (1-k) + ClusteringSimModel::CLUSTER_RADIUS * k * rp;
}

namespace ifucm
{
	FuzzyCostComputor* fcc = new FuzzyCostComputor();
}
