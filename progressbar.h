/***************************************************************************
 *   Copyright (C) 2007 by Lucas V. Hartmann <lucas.hartmann@gmail.com>.   *
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

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include "stdint.h"
#include "unistd.h"

class progressBar {
	unsigned long total, current;
	unsigned long lastUpdated;
	double tsStart, tsLastUpdated;
	std::string msg;

	unsigned screenWidth;

	public:
	static const uint32_t NPOS;

	// Constructor 1, from the total number of points
	progressBar(uint32_t nt=100);

	// Constructor 2, from a message and the total number of points
	progressBar(std::string m, uint32_t nt=100);

	// Change current position
	progressBar &operator() (uint32_t c=NPOS) {
		current = c==NPOS ? total : c;
		return *this;
	}
	progressBar &operator++ () {
		if (++current > total) current = total;
		return *this;
	}
	progressBar &operator++ (int) {
		if (++current > total) current = total;
		return *this;
	}

	void reset();
	void reset(uint32_t nt);

	// Display the progressbar in the ostream (expected to be a terminal)
	std::ostream &display(std::ostream &out);
};

inline std::ostream &operator<<(std::ostream &out, progressBar &b) {
	return b.display(out);
}

#endif
