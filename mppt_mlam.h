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

#ifndef MPPT_MLAM_H
#define MPPT_MLAM_H

#include <bilinear.h>

struct mppt_mlam {
	private:
	bilinear_interpolator bil;
	
	public:
	// Parâmetros para G=1000W/m^2
	double Iphr; // Corrente fotovoltaica
	double mr;   // Fator de Idealidade do diodo
	double Rs;   // Resistência série equivalente do gerador
	double Rp;   // Resistência paralela equivalente do gerador
	double Ior;  // Corrente do diodo
	double Tr;   // Temperatura de referência
	int    Ns;   // N[umero de células em série
	double operator () (double I, double T) const { return bil(I,T); } // Calcula a tensão de referência
	
	void setMap(double minI, double maxI, int nI, double minT, double maxT, int nT);
	operator bool() const { return bil; }
	mppt_mlam();
};

#endif
