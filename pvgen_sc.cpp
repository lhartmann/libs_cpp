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
#include "pvgen_sc.h"

double pvGenerator_sc::g(
	const pvGenerator_sc::model_parameters_t &m,
	double V, double I
) {
	double Vt = m.T * K/q;
	return -I+(V-m.Rs*I)*(1/m.Rp+m.I0/(m.m*Vt)*std::exp((V+m.Rs*I)/(m.m*Vt)));
}

double pvGenerator_sc::dgdv(
	const pvGenerator_sc::model_parameters_t &m,
	double V, double I
) {
	double Vt = m.T * K/q;
	return m.I0/(m.m*Vt)*((V+m.Rs*I)/(m.m*Vt)+1)*std::exp((V+m.Rs*I)/(m.m*Vt))+1/m.Rp;
}

double pvGenerator_sc::dgdi(
	const pvGenerator_sc::model_parameters_t &m,
	double V, double I
) {
	double Vt = m.T * K/q;
	return ((V-m.Rs*I)/(m.m*Vt)-1)*m.Rs*m.I0/(m.m*Vt)*std::exp((V+m.Rs*I)/(m.m*Vt))-m.Rs/m.Rp-1;
}

double pvGenerator_sc::V(double I, double vn) const {
	return pvGenerator::V(curmdl, I, vn);
}

double pvGenerator_sc::I(double V, double in) const {
	return pvGenerator::I(curmdl, V, in);
}

/*double pvGenerator_sc::Vmp(double Imp) const { // Resolve V de I
	double vo=25, vn=0;
	double yo=g(vo,Imp);
	int itr=itrLimit;
	// x -= Y dx/dy
	while (itr--) {
		double yn = g(vn,Imp);
		double d = yn*(vn-vo)/(yn-yo);
		yo=yn; vo=vn; vn-=d;
		
		if (fabs(d)<eMax) return vn;
	}
	return -1;
}//*/
double pvGenerator_sc::Vmp(double Imp) const { // Resolve V de I
	double vo=-100, vn=100;
	int itr=itrLimit;
	while (itr--) {
		vn -= g(curmdl,vn,Imp)/dgdv(curmdl,vn,Imp);
		if (fabs(vn-vo)<eMax) return vn;
		vo=vn;
	}
	return -1;
}//*/
double pvGenerator_sc::Imp(double Vmp) const { // Resolve I de V
	double io=25, in=0;
	double yo=g(curmdl, Vmp, io);
	int itr=itrLimit;
	// x -= Y dx/dy
	while (itr--) {
		double yn = g(curmdl, Vmp, in);
		double d = yn*(in-io)/(yn-yo);
		yo=yn; io=in; in-=d;
		
		if (fabs(d)<eMax) return in;
	}
	return -1;
}
/*double pvGenerator::Imp(double Vmp) const { // Resolve I de V
	double io=-100, in=0;
	int itr=itrLimit;
	while (itr--) {
		in -= g(Vmp,in)/dgdi(Vmp,in);
		if (fabs(in-io)<eMax) return in;
		io=in;
	}
	return -1;
}*/
