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

#include "pvgen_sc.h"
#include "pvgen_model_test.h"
#include "pvgen_setup.h"
#include "pvgen_nominal_model.h"

#define DEBUG
#include "debug.h"
#include <iostream>
#include <iomanip>

using namespace std;

int pvgen_model_test(const pvgen_parameters_t *genparam, const char *outfilename) {
	cout<<"Testing PV generator mathematical model..."<<endl;
	
	// Prepare generator model (use experimental model)
	cout<<"Preparing PV generator models ("<<genparam->name<<")... "<<flush;
	pvGenerator_sc fitted, nominal;
	fitted.setModel(genparam->model);
	pvgen_model_parameters_t m = pvgen_nominal_model(genparam->nameplate);
	
// 	m.Iphr = 8.0210333509783;
// 	m.Ior  = 1.2248535228464e-6;
// 	m.mr   = 54.1445963664748;
// 	m.Rs   = 0.0955610709405;
// 	m.Rp   = 820.3068866075569;
	
	nominal.setModel(m);
	cout<<"Ok."<<endl;
	
	// Set both math models to the nominal testing condition
	fitted.setInsolation(genparam->nameplate.Gr);
	fitted.setTemperature(genparam->nameplate.Tr);
	nominal.setInsolation(genparam->nameplate.Gr);
	nominal.setTemperature(genparam->nameplate.Tr);
	
	// Debug_dumps
	debug_say("Parameters for "<<genparam->name<<":");
	debug_say("  Nominal:");
	debug_say("    Voc = " << genparam->nameplate.Voc);
	debug_say("    Isc = " << genparam->nameplate.Isc);
	debug_say("    Vmp = " << genparam->nameplate.Vmp);
	debug_say("    Imp = " << genparam->nameplate.Imp);
	debug_say("    Pmp = " << genparam->nameplate.Vmp*genparam->nameplate.Imp);
	debug_say("    Ns  = " << genparam->nameplate.Ns);
	debug_say("    Gr  = " << genparam->nameplate.Gr);
	debug_say("    Tr  = " << genparam->nameplate.Tr);
	debug_say("    Iph = " << nominal.getSourceCurrent());
	debug_say("    m   = " << nominal.getDiodeIdealityFactor());
	debug_say("    Rs  = " << nominal.getRs());
	debug_say("    Rp  = " << nominal.getRp());
	debug_say("    Io  = " << nominal.getDiodeCurrentGain());
	
	if (true) {
		// Set both math models to the experimental testing condition
		fitted.setInsolation(genparam->model.G);
		fitted.setTemperature(genparam->model.T);
		nominal.setInsolation(genparam->model.G);
		nominal.setTemperature(genparam->model.T);
	}
	
	// Create output file
	cout<<"Creating output file ("<<outfilename<<")... "<<flush;
	ofstream out(outfilename, ios::out|ios::trunc);
	if (!out) {
		cout<<"Error."<<endl;
		cerr<<"Failed to create output file \""<<outfilename<<"\"."<<endl;
		return 1;
	}
	cout<<"Ok."<<endl;
	
	// Run the test
	cout<<"Running test... "<<flush;
	out<<"V If In"<<endl;
	double V = 0, If = 1, In = 1;
	double Pmpf = -1, Vmpf = 0, Impf = 0;
	do {
		If = fitted.I(V);
		In = nominal.I(V);
		out<<V<<" "<<If<<" "<<In<<endl;
		
		if (Pmpf < V*If) {
			Vmpf = V;
			Impf = If;
			Pmpf = Vmpf*Impf;
		}
		
		V += 0.01;
	} while (If >= 0 || In >= 0);
	cout<<"Done."<<endl;
	
	// Dump fitted model parameters
	debug_say("  Fitted:");
	debug_say("    Voc = " << fitted.V(0));
	debug_say("    Isc = " << fitted.I(0));
	debug_say("    Vmp = " << Vmpf);
	debug_say("    Imp = " << Impf);
	debug_say("    Pmp = " << Pmpf);
	debug_say("    Ns  = " << fitted.getSeriesCellCount());
	debug_say("    Gr  = " << fitted.getInsolation());
	debug_say("    Tr  = " << fitted.getTemperature());
	debug_say("    Iph = " << fitted.getSourceCurrent());
	debug_say("    m   = " << fitted.getDiodeIdealityFactor());
	debug_say("    Rs  = " << fitted.getRs());
	debug_say("    Rp  = " << fitted.getRp());
	debug_say("    Io  = " << fitted.getDiodeCurrentGain());
	
	return 0;
}
