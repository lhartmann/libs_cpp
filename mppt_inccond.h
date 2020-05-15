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

#ifndef MPPT_INCCOND_H
#define MPPT_INCCOND_H

struct mppt_inccond {
	double Va, Ia, Vr; // Tensão e corrente no passo anterior
	double dVr; // Passo de tensão
	mppt_inccond() : Va(0), Ia(0), Vr(0), dVr(0.1) {}
	double operator () (double V, double I); // Calcula a tensão de referência
};

inline double mppt_inccond::operator () (double V, double I) {
	double dV = V - Va;
	double dI = I - Ia;
	double dP = V*I - Va*Ia;
	Va = V;
	Ia = I;
	if (dV == 0) {
		if (dI>0) return Vr+=dVr;
		if (dI<0) return Vr-=dVr;
		return Vr;
	}
	if (dI/dV > -I/V) return Vr+=dVr;
	if (dI/dV < -I/V) return Vr-=dVr;
	return Vr;
}

#endif
