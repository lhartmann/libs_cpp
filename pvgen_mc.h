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

//pvGenerator.h
#ifndef PVGEN_MC_H
#define PVGEN_MC_H
#include <vector>
#include "pvgen.h"

class pvGenerator_mc : public pvGenerator {
	std::vector<model_parameters_t> cell;
	
	void updateCells();
	
	public:
	pvGenerator_mc() {
		// Do nothing
	}
	void setNs(int Ns);
	void setSourceReference(double Iph, double G);
	void setDiodeModel(double I0, double T, double m);
	
	// Set all cells
	void setInsolation(double G);
	void setTemperature(double T);
	
	// Set single cell
	void setInsolation(int i, double G);
	void setTemperature(int i, double T);
	
	void setSeriesCellCount(int Ns) { setNs(Ns); }
	void setRs(double Rs);
	void setRp(double Rp);
	void setModel(const model_parameters_t &m);
	
	double getInsolation(int i) const {
		if (i<0 || i>=cell.size()) return NAN;
		return cell[i].G;
	}
	double getTemperature(int i) const {
		if (i<0 || i>=cell.size()) return NAN;
		return cell[i].T;
	}
	
	double V(double I, double v_old=0) const; // Resolve V de I
	double I(double V, double i_old=1) const; // Resolve I de V
};

#endif
