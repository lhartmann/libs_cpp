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
#ifndef PVGEN_H
#define PVGEN_H
#include <cmath>

// Temporary code to prevent breaks
#define pvgen_nameplate_parameters_t pvGenerator::nameplate_parameters_t
#define pvgen_model_parameters_t     pvGenerator::model_parameters_t
#define pvgen_parameters_t           pvGenerator::parameters_t

class pvGenerator {
public:
	struct nameplate_parameters_t {
		int Ns;
		double Voc, Isc, Vmp, Imp, Gr, Tr;
		double kT_Voc, kT_Isc;
	};
	struct model_parameters_t {
		int Ns;
		double Iph, I0, m, Rs, Rp, G, T;
	};
	struct parameters_t {
		char *name;
		nameplate_parameters_t nameplate;
		model_parameters_t model;
	};
	
protected:
	static const double q;
	static const double K;
	static const double e;
	
	// Iterative solution configurations
	int itrLimit;  // Iteration count limit
	double eMax;   // Error limit
	
	model_parameters_t refmdl; // Reference model
	model_parameters_t curmdl; // Current model
	
	// Build dst from src using selected temperature and radiation.
	static void fix(const model_parameters_t &src, model_parameters_t &dst, double G, double T);
	
	// Objective function for numeric solver
	static double f(const model_parameters_t &m, double V, double I);
	// Objective function derivatives
	static double dfdv(const model_parameters_t &m, double V, double I);
	static double dfdi(const model_parameters_t &m, double V, double I);
	
	double V(const model_parameters_t &m, double I, double v_old=0) const;
	double I(const model_parameters_t &m, double V, double i_old=0) const;
	
public:
	pvGenerator();
	virtual ~pvGenerator() {}
	
	virtual void setIterationParameters(int nil, double e);
	virtual void setNs(int Ns);
	virtual void setSourceReference(double Iph, double G);
	virtual void setDiodeModel(double I0, double T, double m);
	virtual void setInsolation(double G);
	virtual void setTemperature(double T);
	virtual void setSeriesCellCount(int Ns);
	virtual void setRs(double Rs);
	virtual void setRp(double Rp);
	virtual void setModel(const model_parameters_t &m);
	virtual model_parameters_t getReferenceModel() const { return refmdl; }
	virtual model_parameters_t getModel() const { return curmdl; }
	
	virtual double V(double I, double v_old=0) const = 0; // Resolve V de I
	virtual double I(double V, double i_old=0) const = 0; // Resolve I de V
	
	// Readbacks
	// Reference values
	virtual double getSourceCurrentReference() const;
	virtual double getInsolationReference() const;
	virtual double getDiodeCurrentGainReference() const;
	virtual double getTemperatureReference() const;
	virtual double getThermalVoltageReference() const;
	// Current values
	virtual double getSourceCurrent() const;
	virtual double getRs() const;
	virtual double getRp() const;
	virtual double getDiodeCurrentGain() const;
	virtual double getThermalVoltage() const;
	virtual double getDiodeIdealityFactor() const;
	virtual double getInsolation() const;
	virtual double getTemperature() const;
	virtual int getSeriesCellCount() const;
	//
	virtual int getIterationCountLimit() const;
	virtual double getIterationErrorLimit() const;
};

#endif
