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
 **/

#include "MatFile.h"

#include <cstdio>
#include <iostream>
#include <cstring>

MatFile::MatFile() : file(-1)
{
}
MatFile::MatFile(const std::string& fn) : file(-1)
{
	Open(fn);
}

MatFile::~MatFile()
{
	Close();
}

bool MatFile::Open(const std::string& fn)
{
	if (file >= 0)
		Close();

	filename = fn;

	file = H5Fcreate(fn.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	return file >= 0;
}

bool MatFile::Close()
{
	if (file >= 0)
	{
		H5Fclose(file);
		file = -1;
		return PrependHeader(filename.c_str());
	}
	return false;
}

bool MatFile::IsOpen() const
{
	return file >= 0;
}

bool MatFile::WriteMatrix(const std::string& varname, int nx, int ny, const double* data)
{
	if (file < 0)
		return false;

	if (nx <= 0 || ny <= 0)
		return true;

	hsize_t dimsf[2];
	dimsf[0] = nx;
	dimsf[1] = ny;
	return H5LTmake_dataset_double(file, varname.c_str(), 2, dimsf, data) >= 0;
}

bool MatFile::WriteVector(const std::string& varname, int nx, const double* data)
{
	if (file < 0)
		return false;

	if (nx <= 0)
		return true;

	hsize_t dimsf[1];
	dimsf[0] = nx;
	return H5LTmake_dataset_double(file, varname.c_str(), 1, dimsf, data) >= 0;
}



bool MatFile::PrependHeader(const char* filename)
{
	// Prepend the header.
	FILE* fd = fopen(filename, "r+b");
	if (!fd)
	{
		std::cerr << "Couldn't open file for writing header." << std::endl;
		return false;
	}

	char header[512];
	memset(header, 0, sizeof(header));
	sprintf(header, "MATLAB 7.3 format file written by MatFile class, by Tim Hutt"); // Do not make this longer than 116 chars (I think)
	header[124] = 0;
	header[125] = 2;
	header[126] = 'I';
	header[127] = 'M';

	// Get file length.
	fseek(fd, 0, SEEK_END);
	long long length = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	// TODO: Do this properly without reading entire file into memory.
	if (length > 1024L*1024L*1024L) // 10 GB.
	{
		std::cerr << "File too big to write header, sorry.";
		fclose(fd);
		return false;
	}

	unsigned char* buffer = new unsigned char[length];
	if (fread(buffer, 1, length, fd) != length)
	{
		delete[] buffer;
		std::cerr << "Couldn't read file, sorry.";
		fclose(fd);
		return false;
	}

	fseek(fd, 0, SEEK_SET);

	fwrite(header, 1, sizeof(header), fd);
	fwrite(buffer, 1, length, fd);
	fclose(fd);

	delete[] buffer;

	return true;
}
