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
#include "pvgen.h"

// Static constants
const double pvGenerator::q = 1.602177e-19;
//const double pvGenerator::K = 1.380658e-23;
const double pvGenerator::K = 1.3854e-23;
const double pvGenerator::e = 1.12;

// Build dst from src using selected temperature and radiation.
void pvGenerator::fix(
	const pvGenerator::model_parameters_t &src,
	pvGenerator::model_parameters_t &dst,
	double G, double T
) {
	dst = src;
	dst.G = G;
	dst.T = T;
	
	dst.Iph = src.Iph * G / src.G;
	double sVt = src.T * K/q;
	double dVt = dst.T * K/q;
	dst.I0 = src.I0*pow(T/src.T,3)*std::exp(e/(src.m/src.Ns)*(1/sVt-1/dVt));
}
	
// Objective function for numeric solver
double pvGenerator::f(
	const pvGenerator::model_parameters_t &m,
	double V, double I
) {
	double Vt = m.T * K/q;
	return m.Iph-I-m.I0*(std::exp((V+m.Rs*I)/(m.m*Vt))-1)-(V+m.Rs*I)/m.Rp;
}

// Objective function derivatives
double pvGenerator::dfdv(
	const pvGenerator::model_parameters_t &m,
	double V, double I
) {
	double Vt = m.T * K/q;
	return -m.I0/(m.m*Vt)*std::exp((V+m.Rs*I)/(m.m*Vt))-1/m.Rp;
}

double pvGenerator::dfdi(
	const pvGenerator::model_parameters_t &m,
	double V, double I
) {
	double Vt = m.T * K/q;
	return -1-(m.I0*m.Rs)/(m.m*Vt)*std::exp((V+m.Rs*I)/(m.m*Vt))-m.Rs/m.Rp;
}

#define V_FROM_I_NEWTON
double pvGenerator::V(
	const model_parameters_t &m,
	double I, double vn
) const {
#if defined V_FROM_I_NEWTON
	double vo=-100;
	int itr=itrLimit;
	while (itr--) {
		vn -= f(m,vn,I)/dfdv(m,vn,I);
		if (fabs(vn-vo)<fabs(eMax*vo)) return vn;
		vo=vn;
	}
#else
	int itr=itrLimit;
	double V1 = vn;
	double V0 = vn + 1;
	double Y1 = f(m,V1,I);
	double Y0 = f(m,V0,I);
	while (itr--) {
		double V2 = V1 - Y1 * (V1-V0) / (Y1-Y0);
		if (fabs(V2-V1)<fabs(eMax*V1)) return V2;
		V0=V1; Y0=Y1;
		V1=V2; Y1=f(m,V1,I);
	}
#endif
	return NAN;
}

double pvGenerator::I(
	const model_parameters_t &m,
	double V, double in
) const {
	double io=-100;
	int itr=itrLimit;
	while (itr--) {
		in -= f(m,V,in)/dfdi(m,V,in);
		if (fabs(in-io)<fabs(eMax*io)) return in;
		io=in;
	}
	return NAN;
}

pvGenerator::pvGenerator() {
	itrLimit=100; eMax=1e-7;
	refmdl.Iph = 1;
	refmdl.G   = 1000;
	refmdl.I0  = 1;
	refmdl.m   = 1;
	refmdl.T   = 273.16 + 25;
	refmdl.Rp  = 1;
	refmdl.Rs  = 1;
	curmdl = refmdl;
}

void pvGenerator::setIterationParameters(int nil, double e) {
	itrLimit = nil;
	eMax = e;
}

void pvGenerator::setNs(int Ns) {
	refmdl.Ns = Ns;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
}

void pvGenerator::setSourceReference(double Iph, double G) {
	refmdl.Iph = Iph;
	refmdl.G   = G;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
}

void pvGenerator::setDiodeModel(double I0, double T, double m) {
	refmdl.I0 = I0;
	refmdl.T  = T;
	refmdl.m  = m;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
}

void pvGenerator::setInsolation(double G) {
	fix(refmdl, curmdl, G, curmdl.T);
}

void pvGenerator::setTemperature(double T) {
	fix(refmdl, curmdl, curmdl.G, T);
}

void pvGenerator::setSeriesCellCount(int Ns) {
	setNs(Ns);
}

void pvGenerator::setRs(double Rs) {
	refmdl.Rs = Rs;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
}

void pvGenerator::setRp(double Rp) {
	refmdl.Rp = Rp;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
}

void pvGenerator::setModel(const model_parameters_t &m) {
	refmdl = m;
	fix(refmdl, curmdl, curmdl.G, curmdl.T);
}

// Readbacks
// Reference values
double pvGenerator::getSourceCurrentReference() const { return refmdl.Iph; }
double pvGenerator::getInsolationReference() const { return refmdl.G; }
double pvGenerator::getDiodeCurrentGainReference() const { return refmdl.I0; }
double pvGenerator::getTemperatureReference() const { return refmdl.T; }
double pvGenerator::getThermalVoltageReference() const { return refmdl.T * K/q; }
// Current values
double pvGenerator::getSourceCurrent() const { return curmdl.Iph; }
double pvGenerator::getRs() const { return curmdl.Rs; }
double pvGenerator::getRp() const { return curmdl.Rp; }
double pvGenerator::getDiodeCurrentGain() const { return curmdl.I0; }
double pvGenerator::getThermalVoltage() const { return curmdl.T * K/q; }
double pvGenerator::getDiodeIdealityFactor() const { return curmdl.m; }
double pvGenerator::getInsolation() const { return curmdl.G; }
double pvGenerator::getTemperature() const { return curmdl.T; }
int pvGenerator::getSeriesCellCount() const { return curmdl.Ns; }
//
int pvGenerator::getIterationCountLimit() const { return itrLimit; }
double pvGenerator::getIterationErrorLimit() const { return eMax; }
