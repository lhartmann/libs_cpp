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

#include "leastsquares.h"
#include <cmath>
// #define DEBUG
#include "debug.h"

double lsqPolyVal(double x, std::vector<double> p) {
	double y = 0;
	double a = 1;
	for (int i=0; i<p.size(); ++i) {
		y += p[i] * a;
		a *= x;
	}
	return y;
}

std::vector<double> least_squares(
	std::vector<double> x,
	std::vector<double> y,
	int order
) {
	std::vector<double> w(x.size());
	for (int i=0; i<w.size(); ++i) w[i]=1;
	return weighed_least_quares(x,y,w,order);
}

std::vector<double> weighed_least_quares(
	std::vector<double> x,
	std::vector<double> y,
	std::vector<double> w,
    int order
) {	
	using std::vector;
	++order;
	if (order < 1       )     return vector<double>();
	if (x.size() < order)     return vector<double>();
	if (x.size() != y.size()) return vector<double>();
	if (x.size() != w.size()) return vector<double>();
	
	// Column-major makes my life easier...
	//   R holds a square matrix plus a column, "row-reduced echelon format".
	
	std::vector< std::vector<double> > R(order + 1); 
	for (int j=0; j<=order; ++j) R[j].resize(order);
	
	// Build required matrices
	for (int i=0; i<order; ++i) {
		for (int j=0; j<order; ++j) {
			R[j][i] = 0;
			for (int k=0; k<x.size(); ++k) {
				R[j][i] += std::pow(x[k],i+j) * w[k];
			}
//			R[j][i] *= w[i];
		}
		for (int k=0; k<x.size(); ++k) {
			R[order][i] += std::pow(x[k],i) * y[k] * w[k];
		}
	}
	
	//
	#ifdef DEBUG
	debug_say("RREF matrix");
	for (int i=0; i<order; ++i) {
		for (int j=0; j<=order; ++j) {
			DEBUG_STREAM<<R[j][i]<<"     ";
		}
		DEBUG_STREAM<<std::endl;
	}
	#endif
	
	// Matrix inversion.
	for (int i=0; i<order; ++i) { // Diagonal element
		for (int j=0; j<order; ++j) { // Working row
			if (i==j) continue;
			double G = -R[i][j] / R[i][i];
			for (int k=0; k<=order; ++k) { // Working column
				R[k][j] += G * R[k][i];
			}
		}
	}
	
	#ifdef DEBUG
	debug_say("RREF after inversion");
	for (int i=0; i<order; ++i) {
		for (int j=0; j<=order; ++j) {
			DEBUG_STREAM<<R[j][i]<<"     ";
		}
		DEBUG_STREAM<<std::endl;
	}
	#endif

	for (int i=0; i<order; ++i) {
		R[order][i] /= R[i][i];
	}

	// This is why I used column-major:
	return R[order];
}

double lsqRmsError(
	std::vector<double> x,
	std::vector<double> y,
	std::vector<double> p
) {
	std::vector<double> w(x.size());
	for (int i=0; i<w.size(); ++i) w[i]=1;
	return lsqWeighedRmsError(x,y,w,p);
}

double lsqWeighedRmsError(
	std::vector<double> x,
	std::vector<double> y,
	std::vector<double> w,
	std::vector<double> p
) {
	double e = 0;
	double W = 0;
	if (x.size() != y.size()) return NAN;
	if (x.size() != w.size()) return NAN;
	for (int i=0; i<x.size(); ++i) {
		double f = y[i];
		double a = 1;
		for (int j=0; j<p.size(); ++j) {
			f -= p[j] * a;
			a *= x[i];
		}
		e += w[i] * f*f;
		W += w[i];
	}
	return std::sqrt(e / W);
}
