#include "monitor.h"

Monitor::Monitor()
{
}

Monitor::~Monitor()
{
}

#ifdef _MATLAB_
bool Monitor::wirte_to_mat(const string& file_path, const string& var_name, const double* array, int row_num, int col_num)
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
#endif

#ifndef _MATLAB_

bool Monitor::wirte_to_mat(const string& file_path, const string& var_name, const double* array, int row_num, int col_num)
{
	MatFile* matfile = new MatFile(file_path);
	bool result = matfile->WriteMatrix(var_name, col_num, row_num, array);
	delete matfile;
	return result;
}
#endif

