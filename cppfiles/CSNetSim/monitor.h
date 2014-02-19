#ifndef MONITOR_H
#define MONITOR_H

#include "compile_config.h"

#include <string>
using namespace std;

class Monitor
{
public:
	Monitor();
	virtual ~Monitor();
public:
	virtual void record_in_run() {};
	virtual void record_before_run() {};
	virtual void record_after_run() {};
public:
	bool wirte_to_mat(string file_path, string var_name, double* array, int row_num, int col_num);
};

#endif // MONITOR_H
