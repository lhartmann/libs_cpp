/***************************************************************************
 *   Copyright (C) 2009 by Lucas V. Hartmann <lucas.hartmann@gmail.com>.   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SCILABDAT_H
#define SCILABDAT_H

#include <stdint.h>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>

struct scilabdathdr_t {
	uint8_t name[24];
	uint32_t type;
	uint32_t rows;
	uint32_t cols;
	uint32_t imagf;
};

inline uint32_t scilabdat_type(double) { return 1; }

static uint32_t scilabdat_header(std::ofstream &file, const char *name, uint32_t type, bool imag, int rows, int cols) {
	scilabdathdr_t header;
	header.type = type;
	header.rows = rows;
	header.cols = cols;
	header.imagf = imag?1:0;
	
	int nl = std::strlen(name);
	if (nl > sizeof(header.name)) return -1;
	for (int i=0; i<sizeof(header.name); ++i) {
		header.name[i] = 40; // Blank
	}
	bool wasupper = false;
	for (int i=0; i<nl; ++i) {
		uint8_t c = name[i];
		if      (c == '_')   { header.name[i] = 0x24           - wasupper; wasupper = false; }
		else if (isdigit(c)) { header.name[i] = 0x00 + (c-'0') - wasupper; wasupper = false; }
		else if (isupper(c)) { header.name[i] = 0xF6 - (c-'A') - wasupper; wasupper = true;  }
		else if (islower(c)) { header.name[i] = 0x0A + (c-'a') - wasupper; wasupper = false; }
		else return -1;
	}
	if (wasupper && nl<sizeof(header.name)) header.name[nl]--;

	uint32_t offset = file.tellp();
	file.write((char*)&header,sizeof(header));

	return offset;
}

template <class T> static uint32_t scilabdat_add(std::ofstream &file, const char *name, const T *const *real, const T *const *imag, int rows, int cols) {
	uint32_t offset = scilabdat_header(file, name, scilabdat_type(T()), bool(imag), rows, cols);
	if (offset == uint32_t(-1)) return offset;
	
	for (int j=0; j<cols; ++j) for (int i=0; i<rows; ++i) file.write((char*)&real[i][j],sizeof(T));
	if (imag) for (int j=0; j<cols; ++j) for (int i=0; i<rows; ++i) file.write((char*)&imag[i][j],sizeof(T));

	return offset;
}

// Complex 2D array
template <class T> static uint32_t scilabdat_add(std::ofstream &file, const char *name, const T *real, const T *imag, int npt) {
	uint32_t offset = scilabdat_header(file, name, scilabdat_type(T()), bool(imag), npt, 1);
	if (offset == uint32_t(-1)) return offset;
	
	file.write((char*)real,npt*sizeof(T));
	if (imag) file.write((char*)imag,npt*sizeof(T));

	return offset;
}

// Real 2D array
template <class T> static uint32_t scilabdat_add(std::ofstream &file, const char *name, const T *const *real, int rows, int cols) {
	return scilabdat_add(file,name,real,(T**)0,rows,cols);
}

// Real vector
template <class T> static uint32_t scilabdat_add(std::ofstream &file, const char *name, const T *real, int npt) {
	return scilabdat_add(file,name,real,(T*)0,npt);
}
template <class T> static uint32_t scilabdat_add(std::ofstream &file, const char *name, std::vector<T> real) {
	uint32_t offset = scilabdat_header(file, name, scilabdat_type(T()), false, real.size(), 1);
	if (offset == uint32_t(-1)) return offset;
	
	for (int i=0; i<real.size(); ++i) {
		file.write((char*)&real[i], sizeof(T));
	}
	return offset;
}

// Complex scalar
template <class T> static uint32_t scilabdat_add(std::ofstream &file, const char *name, T real, T imag) {
	return scilabdat_add(file,name,&real,&imag,1);
}

// Real scalar
template <class T> static uint32_t scilabdat_add(std::ofstream &file, const char *name, T real) {
	return scilabdat_add(file,name,&real,(T*)(0),1);
}

#endif
