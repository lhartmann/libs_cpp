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

#include "pvgen_models.h"
#include <cstring>

class pvgen_parameters_loader_t {
	pvgen_parameters_t *generators;
public:
	pvgen_parameters_loader_t();
	~pvgen_parameters_loader_t();
};

const pvgen_parameters_t *generators = 0;
static pvgen_parameters_loader_t loader;

pvgen_parameters_loader_t::pvgen_parameters_loader_t() {
	generators = new pvgen_parameters_t[GEN_COUNT];
	if (!generators) return;
	
	// Kyocera KC130TM
	generators[GEN_KC130TM].name = "KC130TM";
	generators[GEN_KC130TM].nameplate.Ns = 36;
	generators[GEN_KC130TM].nameplate.Voc = 21.9;
	generators[GEN_KC130TM].nameplate.Isc = 8.02;
	generators[GEN_KC130TM].nameplate.Vmp = 17.6;
	generators[GEN_KC130TM].nameplate.Imp = 7.39;
	generators[GEN_KC130TM].nameplate.Gr = 1000;
	generators[GEN_KC130TM].nameplate.Tr = 25 + 273.16;
	generators[GEN_KC130TM].nameplate.kT_Isc = +3.18e-3;
	generators[GEN_KC130TM].nameplate.kT_Voc = -8.24e-2;
	
// 		This generator provides nameplate data under an additional condition
// 		.nameplate = {
// 			.Ns = 36,
// 			.Voc = ----,
// 			.Isc = ----,
// 			.Vmp = 15.5,
// 			.Imp = 5.94,
// 			.Gr = 800,
// 			.Tr = 47 + 273.16
// 		},
	
	generators[GEN_KC130TM].model.Ns  = 36;
	generators[GEN_KC130TM].model.Iph = 9.224704010772673;     // Fitted
	generators[GEN_KC130TM].model.I0  = 3.113104235299001e-06; // Fitted
	generators[GEN_KC130TM].model.m   = 48.618847149653462;    // Fitted
	generators[GEN_KC130TM].model.Rs  = 0.249377189890937;     // Fitted
	generators[GEN_KC130TM].model.Rp  = 80.405207461703370;    // Fitted
	generators[GEN_KC130TM].model.T   = 273.16 + 49.2;         // Measured
	generators[GEN_KC130TM].model.G   = 9.19 / 8.02 * 1000;    // Estimated = Iscr/8.02 * 1000

// 			.Ns   = 36,
// 			.Iphr = 9.224704186444930,         // Fitted
// 			.Ior  = 3.113101108065247e-06,     // Fitted
// 			.mr   = 50.175344266955769,        // Fitted
// 			.Rs   = 0.249377199758917,         // Fitted
// 			.Rp   = 80.405083931350902,        // Fitted
// 			.Tr   = 273.16 + 39.2,             // Measured
// 			.Gr   = 9.224704186444930 / 8.02 * 1000, // Estimated = Iphr/8.02 * 1000
	
	// Solarex SX80
	generators[GEN_SX80].name = "SX80";
	generators[GEN_SX80].nameplate.Ns = 36;
	generators[GEN_SX80].nameplate.Voc = 21.0;
	generators[GEN_SX80].nameplate.Isc = 5.17;
	generators[GEN_SX80].nameplate.Vmp = 16.8;
	generators[GEN_SX80].nameplate.Imp = 4.75;
	generators[GEN_SX80].nameplate.Gr = 1000;
	generators[GEN_SX80].nameplate.Tr = 25 + 273.16;
	generators[GEN_SX80].nameplate.kT_Isc = 0;
	generators[GEN_SX80].nameplate.kT_Voc = 0;
	generators[GEN_SX80].model.Ns  = 36;
	generators[GEN_SX80].model.Iph = 5.16;
	generators[GEN_SX80].model.I0  = 2.72e-6;
	generators[GEN_SX80].model.m   = 56.06;
	generators[GEN_SX80].model.Rs  = 0.495;
	generators[GEN_SX80].model.Rp  = 122.67;
	generators[GEN_SX80].model.T   = 273.16 + 25;
	generators[GEN_SX80].model.G   = 1000;
	
	// Kyocera KD210GX
	generators[GEN_KD210GX].name = "KD210GX";
	generators[GEN_KD210GX].nameplate.Ns = 54;
	generators[GEN_KD210GX].nameplate.Voc = 33.2;
	generators[GEN_KD210GX].nameplate.Isc = 8.58;
	generators[GEN_KD210GX].nameplate.Vmp = 26.6;
	generators[GEN_KD210GX].nameplate.Imp = 7.90;
	generators[GEN_KD210GX].nameplate.Gr = 1000;
	generators[GEN_KD210GX].nameplate.Tr = 25 + 273.16;
	generators[GEN_KD210GX].nameplate.kT_Isc = 0;
	generators[GEN_KD210GX].nameplate.kT_Voc = 0;
	
//	This generator provides nameplate data under an additional condition
//	generators[GEN_KD210GX].nameplate.Vmp = 23.6;
//	generators[GEN_KD210GX].nameplate.Imp = 6.32;
//	generators[GEN_KD210GX].nameplate.Gr = 800;
//	generators[GEN_KD210GX].nameplate.Tr = 47.9 + 273.16;
	
	generators[GEN_KD210GX].model.Ns  = 54;
	generators[GEN_KD210GX].model.Iph = 0; // Fitted
	generators[GEN_KD210GX].model.I0  = 0; // Fitted
	generators[GEN_KD210GX].model.m   = 0; // Fitted
	generators[GEN_KD210GX].model.Rs  = 0; // Fitted
	generators[GEN_KD210GX].model.Rp  = 0; // Fitted
	generators[GEN_KD210GX].model.T   = 0; // Measured
	generators[GEN_KD210GX].model.G   = 0; // Estimated = Iscr/8.02 * 1000
	
	::generators = generators;
};

pvgen_parameters_loader_t::~pvgen_parameters_loader_t() {
	if (generators) {
		::generators = 0;
		delete [] generators;
		generators = 0;
	}
}

const pvgen_parameters_t *generator_by_name(const char *name) {
	for (int i=0; generators[i].name; ++i) {
		if (!std::strcmp(name, generators[i].name)) {
			return &generators[i];
		}
	}
	return 0;
}
