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

#include "regexpp.h"

regularExpression::regularExpression(std::string s) {
	working=false;
	int n = regcomp(&re, s.c_str(), REG_EXTENDED);
	if (n) {
		char str[1024];
		int i = regerror(n, &re, str, 1024);
		if (i>=0 && i<1024) {
			str[i] = 0;
			error = str;
		} else {
			error = "unknown.";
		}
		regfree(&re);
		return;
	}
	rm = new regmatch_t[re.re_nsub+1];
	if (!rm) {
		regfree(&re);
		error = "Failed to allocate memory for subexpressions.";
		return;
	}
	working=true;
	error = "";
}

regularExpression::~regularExpression() {
	if (working) {
		delete [] rm;
//		regfree(&re);
		working=false;
	}
}

bool regularExpression::operator() (std::string s) {
	last=s;
	if (regexec(&re, s.c_str(), re.re_nsub+1, rm, 0)) {
		// No match or error
		last="";
		return false;
	}
	// Match
	last=s;
	return true;
}

std::string regularExpression::operator [] (int n) const {
	if (last=="" || n<0 || n>re.re_nsub) return "";
	return std::string(
		last,
		rm[n].rm_so,
		rm[n].rm_eo - rm[n].rm_so
	);
}

regularExpression::regularExpression(const regularExpression &src) {
	working = src.working;
	re = src.re;
	last = src.last;
	error = src.error;
	
	rm = new regmatch_t[re.re_nsub+1];
	for (int i=0; i<re.re_nsub; ++i) {
		rm[i] = src.rm[i];
	}
}
