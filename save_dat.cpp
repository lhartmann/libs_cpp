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

#include "save_dat.h"
#include <iomanip>

bool save_dat(std::ostream &out, std::map<std::string, std::vector<double> > data) {
	using namespace std;
	typedef map<string, vector<double> > dat_t;
	
	// Ignore empty data
	if (data.empty()) return true;
	
	// Check for a "Time" entry
	dat_t::iterator         time = data.find("Time");
	if (time == data.end()) time = data.find("time");
	if (time == data.end()) time = data.find("TIME");
	if (time == data.end()) time = data.find("t");
	
	// How many points
	int npt = data.begin()->second.size();
	for (dat_t::iterator i = data.begin(); i != data.end(); ++i) {
		if (i->second.size() != npt) return false;
	}
	
	// Output header
	if (time != data.end()) out << " Time";
	for (dat_t::iterator i = data.begin(); i != data.end(); ++i) {
		if (i == time) continue;
		out << " " << i->first;
	}
	out<<endl;
	
	// Output data
	out<<setprecision(15);
	for (int n=0; n<npt; ++n) {
		bool f = true;
		if (time != data.end()) {
			out << (f?"":" ") << time->second[n];
			f = false;
		}
		for (dat_t::iterator i = data.begin(); i != data.end(); ++i) {
			if (i == time) continue;
			out << (f?"":" ") << i->second[n];
			f = false;
		}
		out<<endl;
	}
	
	return false;
}
