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

#include "load_dat.h"
#include <cstdlib>
#include "straux.h"

//#define DEBUG
#include "debug.h"

std::map<std::string, std::vector<double> > load_dat(std::istream &in) {
	using namespace std;
	typedef map<string, vector<double> > dat_t;
	
	// Read header and variable names
	string line;
	if (!getline(in, line)) {
		debug_say("Could not read data stream.");
		return dat_t();
	}
	vector<string> v = strSplit(line, ' ');
	vector<string> name;
	for (size_t i=0; i<v.size(); ++i) {
		string &s = v[i];
		
		// Skip empty fields
		size_t f = s.find_first_not_of(" \r\n\b\f\t\a");
		size_t l = s.find_last_not_of(" \r\n\b\f\t\a");
		if (f == string::npos) continue;
		
		// Insert new vector
		string field(s, f, l-f+1);
		debug_say("Add field: "<<field);
		name.push_back(field);
	}
	
	// Abort on no-data.
	if (name.empty()) {
		debug_say("Failed reading header.");
		return dat_t();
	}
	
	// Read data
	int npt = 0;
	dat_t data;
	while (getline(in, line)) {
		v = strSplit(line, ' ');
		vector<string>::iterator it = name.begin();
		for (size_t i=0; i<v.size(); ++i) {
			string &s = v[i];
			
			// Skip empty fields
			size_t f = s.find_first_not_of(" \r\n\b\f\t\a");
//			size_t l = s.find_last_not_of(" \r\n\b\f\t\a");
			if (f == string::npos) continue;
			
			// Insert new data point
			if (it == name.end()) {
				debug_say("Too many data fields on line: \""<<line<<"\"");
				debug_say("First not used: \""<<s<<"\"");
			}
			double value = atof(s.c_str());
			debug_say("Adding "<<*it<<"["<<npt<<"] = "<<value<<".");
			data[*it++].push_back(value);
		}
		if (it != name.end()) {
			debug_say("Too few data fields on line: \""<<line<<"\"");
			debug_say("First missing field: \""<<*it<<"\"");
			return dat_t();
		}
		++npt;
	}
	debug_say("Read "<<npt<<" data points.");
	return data;
}
