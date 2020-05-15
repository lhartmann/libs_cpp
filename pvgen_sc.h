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

//pvgen_sc.h
#ifndef PVGEN_SC_H
#define PVGEN_SC_H

#include "pvgen.h"

class pvGenerator_sc : public pvGenerator {
protected:
	static double g(const model_parameters_t &m, double V, double I);
	static double dgdv(const model_parameters_t &m, double V, double I);
	static double dgdi(const model_parameters_t &m, double V, double I);
	
public:
	pvGenerator_sc() {
		// Do nothing
	}
	
	double V(double I, double v_old=0) const; // Resolve V de I
	double I(double V, double i_old=0) const; // Resolve I de V
	double Vmp(double Imp) const; // Resolve Vmp de Imp
	double Imp(double Vmp) const; // Resolve Imp de Vmp
};

#endif
