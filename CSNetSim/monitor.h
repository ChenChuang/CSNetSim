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
#include <fstream>

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
	void write_to_dat(const std::string& file_path, const double* array, int n);
	bool write_to_mat(const std::string& file_path, const std::string& var_name, const double* array, int row_num, int col_num);
};

#endif // MONITOR_H
