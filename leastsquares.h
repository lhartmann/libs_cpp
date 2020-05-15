/***************************************************************************
 *   Copyright (C) 2009 by Lucas V. Hartmann <lucas.hartmann@gmail.com>    *
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

#ifndef LEASTSQUARES_H
#define LEASTSQUARES_H

#include <vector>

/* Least-Squares Implementation
 * 
 * Returned vectors should be used as:
 * 
 * Y = sum( p[i] * pow(x,i) )
 */

double lsqPolyVal(double x, std::vector<double> p);

std::vector<double> least_squares(
	std::vector<double> x,
	std::vector<double> y,
	int order
);

std::vector<double> weighed_least_quares(
	std::vector<double> x,
	std::vector<double> y,
	std::vector<double> w,
	int order
);

double lsqRmsError(
	std::vector<double> x,
	std::vector<double> y,
	std::vector<double> p
);

double lsqWeighedRmsError(
	std::vector<double> x,
	std::vector<double> y,
	std::vector<double> w,
	std::vector<double> p
);

#endif
