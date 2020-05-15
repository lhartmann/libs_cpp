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

#ifndef NOMINAL_GENERATOR_MODEL_H
#define NOMINAL_GENERATOR_MODEL_H

#include "pvgen_models.h"

inline pvgen_model_parameters_t pvgen_nominal_model(pvgen_nameplate_parameters_t n) {
	double k  = 1.3806504e-23;
	double q  = 1.6021765e-19;
	double Vt = k*n.Tr/q;
	pvgen_model_parameters_t m;
	
	m.Iph = n.Isc;
	m.m   = (2*n.Vmp-n.Voc) / (Vt*std::log((m.Iph-n.Imp)/n.Isc) + (Vt*n.Imp)/(m.Iph-n.Imp));
	m.I0  = n.Isc / (std::exp(n.Voc/(m.m*Vt)) - 1);
	m.Rs  = (m.m*Vt*std::log((m.Iph-n.Imp-m.I0)/m.I0)-n.Vmp) / n.Imp;
	m.Rp  = 1e300;
	m.Ns  = n.Ns;
	m.G   = n.Gr;
	m.T   = n.Tr;
	
	return m;
}

#endif
