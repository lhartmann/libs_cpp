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

#ifndef RDTSC_H
#define RDTSC_H

typedef unsigned long long timestamp;

static inline volatile timestamp read_timestamp_counter() {
	union {
		timestamp ull;
		unsigned long ul[2];
	};
	asm(
		"rdtsc"
		: "=d" (ul[1]), "=a" (ul[0])
	);
	return ull;
}

//#define CPU_CLOCK (11.5*150e6/9.) // 1.91 GHz (AMD Mobile Athlon XP 2500+)
#define CPU_CLOCK (2.667e9) // 2.66 GHz (Intel Pentium D)
#define timestamp_to_seconds(v) ((v)/CPU_CLOCK)

#endif
