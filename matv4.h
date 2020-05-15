/***************************************************************************
 *   Copyright (C) 2006 by Lucas V. Hartmann <lucas.hartmann@gmail.com>.   *
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

#ifndef MATV4_H
#define MATV4_H

#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <stdint.h>

struct matv4hdr_t {
	uint32_t type;
	uint32_t rows;
	uint32_t cols;
	uint32_t imagf;
	uint32_t namelen;
};

inline uint32_t matv4_type(double) { return 0; }
inline uint32_t matv4_type(float) { return 10; }
inline uint32_t matv4_type(uint32_t) { return 20; }
inline uint32_t matv4_type(int32_t) { return 30; }
inline uint32_t matv4_type(uint16_t) { return 40; }
inline uint32_t matv4_type(uint8_t) { return 50; }

static uint32_t matv4_header(std::ostream &file, const char *name, uint32_t type, bool imag, int rows, int cols) {
	matv4hdr_t header;
	header.type = type;
	header.rows = rows;
	header.cols = cols;
	header.imagf = imag?1:0;
	header.namelen = std::strlen(name)+1;

	uint32_t offset = file.tellp();
	file.write((char*)&header,sizeof(header));
	file.write(name,header.namelen);

	return offset;
}

template <class T> static uint32_t matv4_add(std::ostream &file, const char *name, const T *const *real, const T *const *imag, int rows, int cols) {
	uint32_t offset = matv4_header(file, name, matv4_type(**real), imag, rows, cols);
	for (int j=0; j<cols; ++j) for (int i=0; i<rows; ++i) file.write((char*)&real[i][j],sizeof(T));
	if (imag) for (int j=0; j<cols; ++j) for (int i=0; i<rows; ++i) file.write((char*)&imag[i][j],sizeof(T));

	return offset;
}

// Complex 2D array
template <class T> static uint32_t matv4_add(std::ostream &file, const char *name, const T *real, const T *imag, int npt) {
	uint32_t offset = matv4_header(file, name, matv4_type(*real), imag, npt, 1);
	file.write((char*)real,npt*sizeof(T));
	if (imag) file.write((char*)imag,npt*sizeof(T));

	return offset;
}

// Real 2D array
template <class T> static uint32_t matv4_add(std::ostream &file, const char *name, const T *const *real, int rows, int cols) {
	return matv4_add(file,name,real,(T**)0,rows,cols);
}

// Real vector
template <class T> static uint32_t matv4_add(std::ostream &file, const char *name, const T *real, int npt) {
	return matv4_add(file,name,real,(T*)0,npt);
}
template <class T> static uint32_t matv4_add(std::ostream &file, const char *name, std::vector<T> real) {
	return matv4_add(file, name, real.data(), real.size());
}

// Complex scalar
template <class T> static uint32_t matv4_add(std::ostream &file, const char *name, T real, T imag) {
	return matv4_add(file,name,&real,&imag,1);
}

// Real scalar
template <class T> static uint32_t matv4_add(std::ostream &file, const char *name, T real) {
	return matv4_add(file,name,&real,(T*)(0),1);
}

// String (not supported, need to figure how to do ir first...)
/* static uint32_t matv4_add(std::ostream &file, const char *name, const char *str) {
	int n = strlen(str);
	int h = matv4_header(file,name,1,false,1,n);
	file.write(str,n);
	return h;
} */

#endif
