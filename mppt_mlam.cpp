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

#include "mppt_mlam.h"
#include <cmath>

using namespace std;

static const double q = 1.60217646e-19;
static const double k = 1.3806503e-23;
static const double e = 1.12;

// Calcula a tensão sobre a curva Imax-Vmax
static double map_builder_fcn (double I, double T, void *p) {
	mppt_mlam *mppt  = (mppt_mlam *)p;
	double Iphr = mppt->Iphr;
	double mr   = mppt->mr;
	double Rs   = mppt->Rs;
	double Rp   = mppt->Rp;
	double Ior  = mppt->Ior;
	double Tr   = mppt->Tr;
	int    Ns   = mppt->Ns;

	double Vtr = k*(Tr+273.16)/q;
	double Vt  = k*(T +273.16)/q;
	double Io  = Ior*pow((T+273.16)/(Tr+273.16),3)*exp(e/(mr/Ns)*(1/Vtr-1/Vt));

	// newton-raphson
	double Vm, Vm1 = 10;
	double a = 2;
	for (int n=0; a >= 0.0000001 && n<10000; ++n) {
		double F = -I + Io/(mr*Vt)*(Vm1-Rs*I)*exp((Vm1+Rs*I)/(mr*Vt)) + (Vm1-Rs*I)/Rp;
		double DF = Io/(mr*Vt)*exp((Vm1+Rs*I)/(mr*Vt))*(1 + (Vm1-Rs*I)/(mr*Vt))+1/Rp;
		Vm = Vm1 - F/DF;
		a = abs(Vm - Vm1);
		Vm1 = Vm;
	}
	return Vm;
}

void mppt_mlam::setMap(double minI, double maxI, int nI, double minT, double maxT, int nT) {
	bil.setFunction(0,0);
	
	if (isnan(Iphr)) return;
	if (isnan(mr  )) return;
	if (isnan(Rs  )) return;
	if (isnan(Rp  )) return;
	if (isnan(Ior )) return;
	if (isnan(Tr  )) return;
	
	bil.setX(minI,maxI,nI);
	bil.setY(minT,maxT,nT);
	bil.setFunction(map_builder_fcn, this);
}

mppt_mlam::mppt_mlam() {
	Iphr = mr = Rs = Rp = Ior = Tr = NAN;
	Ns = 36;
}
