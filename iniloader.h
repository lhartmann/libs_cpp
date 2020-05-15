/***************************************************************************
 *   Copyright (C) 2007 by Lucas Vinicius Hartmann                         *
 *   lucas.hartmann@lhartmann-notebook                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef INILOADER_H
#define INILOADER_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

class iniloader {
	// Used components from standard library
/*	using std::string;
    using std::vector;
    using std::istream;
    using std::ostream; */

	// Core types and variables
	struct key_t { string name, value; };
	struct grp_t { string name; vector<key_t> keys; };
	vector< grp_t > contents;
	
	// File saving related variables
	bool saveOnQuit;
	string fileName;
	
	// Current group
	mutable grp_t *currentGroup;
	
	// Auxiliary functions
	static       key_t *findKey  (      grp_t *g, string name);
	static const key_t *findKey  (const grp_t *g, string name);
	      grp_t *findGroup(string name);
	const grp_t *findGroup(string name) const;
	
	public:
	// Clears all configurations performing full reinitialization.
	void clear();
	
	// Loader functions
	int loadFromFile(string fn, bool save=false);
	void loadFromStream(istream &in);
	
	// Saver functions
	int saveToFile(string fn) const;
	void saveToStream(ostream &out) const;
	
	// Table Lookup functions
	bool hasGroup(string group) const           { return         findGroup(group)     ?true:false; };
	bool hasKey(string key) const               { return findKey(currentGroup    ,key)?true:false; };
	bool hasKey(string group, string key) const { return findKey(findGroup(group),key)?true:false; };
	
//	void setGroup(string group) const; // May only change currentGroup (UNSAFE)
	string getKey(string key) const;
	string getKey(string group, string key) const;
	string operator[] (string key) const { return getKey(key); }
	
	// Table editing functions
	void setGroup(string group); // Changes currentGroup and may add a group if it does not exist.
	void setKey(string key, string Value);
	void setKey(string group, string key, string Value);
	string& operator[] (string key);
	
	// Constructors and destructors
	iniloader() { clear(); }
    iniloader(string fn, bool save=false) { clear(); loadFromFile(fn,save); }
	iniloader(istream &in) { clear(); loadFromStream(in); }
	~iniloader() { if (saveOnQuit && fileName!="") saveToFile(fileName); }

    void patch(iniloader &target) const;

	// Get iteratable data sets
    auto data() const { return contents; }
    auto groupData(std::string group) const { 
		for (auto &g : contents)
			if (g.name == group)
				return g.keys;
		return std::vector<key_t>();
	}
};

#endif
