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

#ifndef PVGEN_SETUP_H
#define PVGEN_SETUP_H

#include "pvgen.h"
#include "pvgen_models.h"
#include "pvgen_nominal_model.h"

inline void pvgen_setup(pvGenerator &gen, const pvgen_model_parameters_t &m) {
	gen.setSeriesCellCount(m.Ns);
	gen.setSourceReference(m.Iph, m.G);
	gen.setDiodeModel(m.I0, m.T, m.m);
	gen.setRs(m.Rs);
	gen.setRp(m.Rp);
	gen.setIterationParameters(1000, 1e-5);
}

inline void pvgen_setup(pvGenerator &gen, const pvgen_nameplate_parameters_t &n) {
	pvgen_setup(gen, pvgen_nominal_model(n));
}

#endif
