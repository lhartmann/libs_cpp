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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <string>

class error {
	protected:
	int line;
	std::string file;
	std::string message;
	
	virtual void output(std::ostream &out);
	
	public:
	error(int l, std::string f, std::string msg);
	friend std::ostream &operator<<(std::ostream &os, error &e);
};

inline std::ostream &operator<<(std::ostream &os, error &e) {
	e.output(os);
	return os;
}

#define mk_error(msg) error(__LINE__, __FILE__, msg)

#endif
