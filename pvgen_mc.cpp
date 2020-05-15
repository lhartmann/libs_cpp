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

// pvGenerator.cpp
#include "pvgen_mc.h"
#include "error.h"

// #define DEBUG
#include "debug.h"

void pvGenerator_mc::setNs(int Ns) {
	refmdl.Ns = Ns;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
	cell.clear();
	updateCells();
}

void pvGenerator_mc::setSourceReference(double Iph, double G) {
	refmdl.Iph = Iph;
	refmdl.G   = G;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
	updateCells();
}

void pvGenerator_mc::setDiodeModel(double I0, double T, double m) {
	refmdl.I0 = I0;
	refmdl.T  = T;
	refmdl.m  = m;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
	updateCells();
}

void pvGenerator_mc::setInsolation(double G) {
	fix(refmdl, curmdl, G, curmdl. T);
	for (int i=0; i<cell.size(); ++i) cell[i].G = G;
	updateCells();
}

void pvGenerator_mc::setTemperature(double T) {
	fix(refmdl, curmdl, curmdl.G, T);
	for (int i=0; i<cell.size(); ++i) cell[i].T = T;
	updateCells();
}

void pvGenerator_mc::setRs(double Rs) {
	refmdl.Rs = Rs;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
	updateCells();
}

void pvGenerator_mc::setRp(double Rp) {
	refmdl.Rp = Rp;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
	updateCells();
}

void pvGenerator_mc::setModel(const model_parameters_t &m) {
	refmdl = m;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
	updateCells();
}

void pvGenerator_mc::updateCells() {
	model_parameters_t c = curmdl;
	c.Ns  = 1;
	c.Rs /= curmdl.Ns;
	c.Rp /= curmdl.Ns;
	c.m  /= curmdl.Ns;
	
	if (cell.size() != curmdl.Ns) {
		// Number of cells changed
		cell.resize(curmdl.Ns);
		for (int i=0; i<cell.size(); ++i) cell[i] = c;
	} else {
		// Number of cells unchanged, preserve insolation/temperature.
		for (int i=0; i<cell.size(); ++i) {
			fix(c, cell[i], cell[i].G, cell[i].T);
		}
	}
}

void pvGenerator_mc::setInsolation(int i, double ng) {
	if (i<0 || i>=cell.size()) {
		debug_say("Cell "<<i<<" is out of range. Available "<<cell.size()<<".");
		throw mk_error("Cell number requested is out of range.");
	}
	model_parameters_t c = cell[i];
	fix(c, cell[i], ng, c.T);
}

void pvGenerator_mc::setTemperature(int i, double nt) {
	if (i<0 || i>=cell.size()) {
		debug_say("Cell "<<i<<" is out of range. Available "<<cell.size()<<".");
		throw mk_error("Cell number requested is out of range.");
	}
	model_parameters_t c = cell[i];
	fix(c, cell[i], c.G, nt);
}

// Solvers

double pvGenerator_mc::V(double I, double vn) const { // Resolve V de I
	double v = 0;
	for (int i=0; i<cell.size(); ++i) v += pvGenerator::V(cell[i], I, vn/cell.size());
	return v;
}


# if 0
// Solve by Newton's method
double pvGenerator_mc::I(double vn, double in) const { // Resolve I de V
	double io=-100;
	int itr=itrLimit;
	while (itr--) {
		double F    = -vn;
		double DFDI = 0;
		for (int i=0; i<cell.size(); ++i) {
			double cV = pvGenerator::V(cell[i], in, vn/cell.size());
			F    += cV;
			DFDI += dfdi(cell[i], cV, in) / dfdv(cell[i], cV, in);
		}
		
		in -= F / DFDI;
		if (fabs(in-io)<fabs(eMax*io)) return in;
		io=in;
	}
	return NAN;
}
#endif

#if 0
// Solve by Pegasus method
double pvGenerator_mc::I(double tV, double nI) const { // Resolve I de V
	int n=itrLimit;
	double oI = 0;
	double oV = V(oI);
	while (n--) {
		double nV = V(nI);
		double dI = -(nV-tV)*(nI-oI)/(nV-oV);
		
		if (nV==tV || fabs(dI)<eMax) return nI;
		
		oI = nI;
		oV = nV;
		nI += dI;
	}
	throw mk_error("Iteration limits exceeded.");
}
#endif

#if 1
// Solve by my descending step method
double pvGenerator_mc::I(double tV, double in) const { // Resolve I de V
	double nI=0, dI=1;
	int n=itrLimit;
	
	for (int i=0; i<cell.size(); ++i) {
		if (dI < cell[i].Iph) dI = cell[i].Iph;
	}
	dI *= 0.55;
	
	while (n--) {
		double nV=V(nI);
		while (std::isnan(nV) && n--) {
			if (nI > 0) nI -= dI/10;
			else        nI += dI/10;
			nV = V(nI);
		}
		if (nV==tV || fabs(dI)<eMax) return nI;
		if (nV>tV) nI += dI;
		else       nI -= dI;
		dI *= 0.55;
	}
	throw mk_error("Iteration limits exceeded.");
}
#endif
