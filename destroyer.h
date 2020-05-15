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
#ifndef DESTROYER_H
#define DESTROYER_H

#include <vector>

/** Task scheduler for when an instance gets out of scope.
 * 
 * This is mostly a helper class for freeing C-style data structures.
 * You may also create adapter functions, and this allows you to schedule
 * any task to when destroyer gets out of scope: Freeing memory, stopping 
 * threads, closing files, closing sockets, and so on.
 * 
 * After allocating a C-stye resource, like
 *   stuff *ptr = (stuff*)malloc(sizeof(stuff));
 * you must remember to free it, like
 *   free(ptr);
 * but I often forget this step, causing horrible memory leaks.
 * 
 * Instead just create an instance of destroyer in the scope you need a
 * given resource, and add the freeing function to it's stack.
 * 
 *   destroyer d;
 *   stuff *ptr = (stuff*)malloc(sizeof(stuff));
 *   d.add(free, ptr);
 *   
 * When destroyer gets out of scope it will automatically free all resources
 * in last-in-first-out order, so the most recently allocated resource gets
 * freed first. 
 */

class destroyer {
	struct FP {
		void (*f)(void*);
		void *p;
		FP(void (*ff)(void*), void *pp) : f(ff), p(pp) {};
	};
	std::vector<FP> v;
	public:
	void clear() {
		while (v.size()) {
			FP x(v.back());
			x.f(x.p);
			v.pop_back();
		}
	}
	destroyer() {
		// Do nothing
	}
	~destroyer() {
		clear();
	}
	void add(void (*f)(void*), void *p) {
		v.push_back(FP(f,p));
	}
};

#endif
