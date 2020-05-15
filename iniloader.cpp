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

#include "iniloader.h"
#include <fstream>

using namespace std;

static string escape(string s) {
	string r;
	for (const auto &i : s) {
		switch (i) {
		case '\n': r+="\\n"; break;
		case '\r': r+="\\r"; break;
		case '\t': r+="\\t"; break;
		case '\f': r+="\\f"; break;
		case '\b': r+="\\b"; break;
		case '\a': r+="\\a"; break;
		case '[' : r+="\\["; break;
		case ']' : r+="\\]"; break;
		case '\\': r+="\\\\"; break;
		case '\'': r+="\\\'"; break;
		case '\"': r+="\\\""; break;
		default:   r+=i;
		}
	}
	return r;
}

static string unescape(string s) {
	string r;
	bool escaped=false;
	for (const auto &i : s) {
		if (escaped) {
			switch (i) {
			case 'n': r+="\n"; break;
			case 'r': r+="\r"; break;
			case 't': r+="\t"; break;
			case 'f': r+="\f"; break;
			case 'b': r+="\b"; break;
			case 'a': r+="\a"; break;
			default:  r+=i;
			}
			escaped=false;
		} else if (i=='\\') escaped=true;
		else r+=i;
	}
	return r;
}

// Auxiliary functions
iniloader::key_t *iniloader::findKey  (grp_t *g, string name) {
	if (!g) return 0;
	vector<key_t>::iterator i;
	for (i=g->keys.begin(); i!=g->keys.end(); ++i) if (i->name==name) return &(*i);
	return 0;
}

const iniloader::key_t *iniloader::findKey  (const grp_t *g, string name) {
	if (!g) return 0;
    for (auto &i : g->keys) if (i.name==name) return &i;
	return 0;
}

iniloader::grp_t *iniloader::findGroup(string name) {
    for (auto &i : contents) if (i.name==name) return &i;
	return 0;
}

const iniloader::grp_t *iniloader::findGroup(string name) const {
    for (auto &i : contents) if (i.name==name) return &i;
	return 0;
}

// Clears all configurations performing full reinitialization.
void iniloader::clear() {
	saveOnQuit = false;
	currentGroup = 0;
}

// Loader functions
int iniloader::loadFromFile(string fn, bool save) {
	saveOnQuit = save;
	ifstream in(fn.c_str(),ios::in);
	if (!in) return 1;
	loadFromStream(in);
	return 0;
}

void iniloader::loadFromStream(istream &in) {
	int ln = 0;
	string blank=" \t\b\r\f\n\a";
	while (true) {
		string s;
		getline(in,s);
		if (!in) break;
		++ln;
		
		size_t first = s.find_first_not_of(blank);
		size_t last  = s.find_last_not_of (blank);
		
		if (first==string::npos || last==string::npos) continue; // Blank line
		
		if (s[first]=='[' && s[last]==']') { // Group header
			string group = string(s, first+1, last - first - 1);
			setGroup(unescape(group));
			continue;
		}
		
/*		size_t eq=first;
		if (s[eq]=='\"') { // Escaped key name
			++eq;
			for (int n=0; eq<==last; ++eq) {
				if (s[eq] == '\\') n++;
				else if (s[eq] == '\"' && n%2==1) break;
				else n=0;
			}
		}
		while (++eq<=last) {
			if (s[eq]=='=') break;
		}
		
		if (eq==string::npos) {
			std::cerr << "Invalid data at line "<<ln<<": "<<s<<endl;;
		}
		*/
		
		size_t eq     = s.find_first_of ('=');
		size_t first2 = s.find_first_not_of(blank,eq+1);
		size_t last2  = s.find_last_not_of (blank,eq-1);
		
		if (eq==string::npos || first2==string::npos || last2==string::npos) {
			std::cerr << "Invalid data at line "<<ln<<": "<<s<<endl;;
		}
		
		string key   = string(s, first,  last2 - first  + 1);
		string value = string(s, first2, last  - first2 + 1);
		
		if (s[first]=='\"' && s[last2]=='\"') key   = unescape(key);
		if (s[first2]=='\"' && s[last]=='\"') value = unescape(value);
		setKey(key, value);
	}
}

// Saver functions
int iniloader::saveToFile(string fn) const {
	ofstream out(fn.c_str());
	if (!out) return 1;
	saveToStream(out);
	if (!out) return 1;
	return 0;
}

void iniloader::saveToStream(ostream &out) const {
	vector<grp_t>::const_iterator gi;
	vector<key_t>::const_iterator ki;
	for (gi = contents.begin(); gi != contents.end(); ++gi) {
		out<<'['<<escape((*gi).name)<<']'<<endl;
		for (ki = (*gi).keys.begin(); ki != (*gi).keys.end(); ++ki) {
			out<<'\"'<<escape((*ki).name)<<"\" = \""<<escape((*ki).value)<<'\"'<<endl;
		}
	}
}

// Table Lookup functions
/* void iniloader::setGroup(string group) const {
	const grp_t *p = findGroup(group);
	if (!p) {
		currentGroup=0;
		return;
	}
	currentGroup = (grp_t*) p; // force const away
} */

string iniloader::getKey(string key) const {
	const key_t *p = findKey(currentGroup, key);
	if (!p) return "";
	return p->value;
}

string iniloader::getKey(string group, string key) const {
	const key_t *p = findKey(findGroup(group), key);
	if (!p) return "";
	return p->value;
}

// Table editing functions
void iniloader::setGroup(string group) {
	grp_t *p = findGroup(group);
	if (!p) {
		grp_t g;
		g.name = group;
		contents.push_back(g);
		p = &(*(contents.end()-1));
	}
	currentGroup = p; // force const away
}

void iniloader::setKey(string key, string value) {
	if (!currentGroup) return;
	key_t *p = findKey(currentGroup, key);
	if (!p) {
		key_t k;
		k.name=key;
		k.value=value;
		currentGroup->keys.push_back(k);
	} else {
		p->value = value;
	}
}

void iniloader::setKey(string group, string key, string value) {
	grp_t *g = findGroup(group);
	if (!g) {
		grp_t tmp;
		tmp.name = group;
		key_t k;
		k.name = key;
		k.value = value;
		tmp.keys.push_back(k);
		contents.push_back(tmp);
	} else {
		key_t *p = findKey(g, key);
		if (!p) {
			key_t k;
			k.name = key;
			k.value = value;
			currentGroup->keys.push_back(k);
		} else {
			p->value = value;
		}
	}
}

string& iniloader::operator[] (string key) {
	static string dummy;
	if (!currentGroup) return dummy="";
	
	key_t *p = findKey(currentGroup, key);
	if (!p) {
		key_t k;
		k.name=key;
		currentGroup->keys.push_back(k);
		p = &(*(currentGroup->keys.end()-1));
	}
	return p->value;
}

void iniloader::patch(iniloader &target) const {
    for (auto g : contents)
        for (auto k : g.keys)
            target.setKey(g.name, k.name, k.value);
}
