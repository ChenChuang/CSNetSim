#ifndef MONITOR_H
#define MONITOR_H

#include "compile_config.h"
#ifdef _MATLAB_
	#include "mex.h"
	#include "mat.h"
#else
	#include "MatFile.h"
#endif

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
	bool wirte_to_mat(const string& file_path, const string& var_name, const double* array, int row_num, int col_num);
};

#endif // MONITOR_H
