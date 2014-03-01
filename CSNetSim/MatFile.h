/*
 * Copyright (c) 2010, Tim Hutt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the distribution
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. 
 */

/*
 * HOW TO:
 * 1. Copy MatFile.h and MatFile.cpp into your source directory;
 * 2. Download and install HDF5. 
 *    1) http://www.hdfgroup.org/ftp/HDF5/current/bin/windows/hdf5-1.8.12-win32-vs10shared.zip 
 *    or 
 *    2) http://www.hdfgroup.org/HDF5/release/obtain5.html;
 * 3. Put "hdf5root/bin" into your project's library paths. Mine is "D:/Program Files/HDF_Group/HDF5/1.8.12/bin";
 * 4. Put "hdf5;hdf5_hl" into your project's library lists;
 * 5. Put "hdf5root\include" into your project's include paths. Mine is "D:\Program Files\HDF_Group\HDF5\1.8.12\include";
 * 6. It's should be OK now.
 **/ 

#ifndef MATFILE_H
#define MATFILE_H

#include "hdf5.h"
#include "hdf5_hl.h"

#include <string>

// Class for writing matlab's .mat files. These are HDF5 files with a simple (and pointless) header.
// Currently only simple matrix and vector variables are supported. TODO: cell arrays would be nice.
class MatFile
{
public:
	// Create an unopened mat file. Use Open after this.
	MatFile();
	// Create and open a mat file.
	explicit MatFile(const std::string& fn);

	// Close the mat file.
	~MatFile();

	// Open a new mat file. If the file already exists it will be overwritten.
	bool Open(const std::string& fn);

	// Close the current file.
	bool Close();

	// Return true if the mat file is open and generally ok.
	bool IsOpen() const;

	// Write a matrix with the given name to the mat file. data is in row major format (i.e. idx = y*width + x).
	bool WriteMatrix(const std::string& varname, int nx, int ny, const double* data);

	// Write a vector.
	bool WriteVector(const std::string& varname, int nx, const double* data);

	// Convenience function.
	bool WriteValue(const std::string& varname, double v)
	{
		return WriteVector(varname, 1, &v);
	}


private:

	// No copy.
	MatFile(const MatFile&);
	const MatFile& operator=(const MatFile&);

	bool PrependHeader(const char* filename);

	std::string filename;
	hid_t file; // -ve is invalid.
};




#endif // MATFILE_H
