/***************************************************************************
 *   Copyright (C) 2007 by Lucas Vinicius Hartmann                         *
 *   lucas.hartmann@gmail.com                                              *
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
#ifndef SMALT_H
#define SMALT_H

// SMALT stands for Simple Memory Allocation Tool

#include <vector>

template <class T> class smalt {
	std::vector<T*> v;
	bool ok;
	public:
	void clear() {
		for (int i=0; i<v.size(); ++i) delete [] v[i];
		v.resize(0);
		ok=true;
	}
	smalt() : ok(true) {
		// Do nothing
	}
	~smalt() {
		clear();
	}
	T *operator() (int n) {
		T *p = new T [n];
		if (p) v.push_back(p);
		else ok = false;
		return p;
	}
	operator bool () { return ok; }
	unsigned long required() { return 0; }
};

#endif
