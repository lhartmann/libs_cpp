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

#ifndef REGEXPP_H
#define REGEXPP_H

#include <regex.h>
#include <string>

class regularExpression {
	bool working;
	regex_t re;
	regmatch_t *rm;
	std::string last;
	std::string error;
	
	public:
	regularExpression(std::string s);
	~regularExpression();
	bool operator() (std::string s);
	std::string operator [] (int n) const;
	
	operator bool() const { return working; }
	int nsubs() const { return re.re_nsub; }
	std::string getLastError() const { return error; }
	
	// Copy operator
	regularExpression(const regularExpression &src);
};

#endif
