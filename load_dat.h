/***************************************************************************
 *   Copyright (C) 2010 by Lucas V. Hartmann <lucas.hartmann@gmail.com>    *
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

#ifndef LOAD_DAT_H
#define LOAD_DAT_H

#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "straux.h"

//#define DEBUG
#include "debug.h"

std::map<std::string, std::vector<double> > load_dat(std::istream &in);

inline std::map<std::string, std::vector<double> > load_dat(std::string filename) {
	debug_say("Opening data file: "<<filename);
	using namespace std;
	ifstream in(filename.c_str(), ios::in);
	return load_dat(in);
}

#endif
