/***************************************************************************
 *   Copyright (C) 2008 by Lucas V. Hartmann <lucas.hartmann@gmail.com>    *
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
#ifndef mppt_temperaturehf_H
#define mppt_temperaturehf_H

#include "mppt_temperature.h"
#include "mppt_inccond.h"

struct mppt_temperaturehf : public mppt_temperature, public mppt_inccond {
	double operator() (double V, double I, double T) {
		return mppt_temperature::operator() (T) 
		         + mppt_inccond::operator() (V,I);
	}
	
	private:
	// Prevent misuse of the conflicting function from both inheritances.
//	double operator() (double, double) {}
};

#endif
