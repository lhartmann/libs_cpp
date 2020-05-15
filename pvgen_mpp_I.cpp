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

#include "pvgen_mpp_I.h"

//#define DEBUG
#include "debug.h"

#if 1

// Scan for the MPP of r, returns the MPP current.
//   Convergence takes ceil(log((Ih-Il)/eMax)/log(2)) iterations.
//   Default values lead to 13 iterations.
double pvgen_mpp_I(pvGenerator &r, double Il, double Ih, double eMax) {
	double I[5] = {
		Il,
		0,
		(Il+Ih)/2,
		0,
		Ih
	};
	double P[5] = {
		r.V(I[0])*I[0],
		0,
		r.V(I[2])*I[2],
		0,
		r.V(I[4])*I[4]
	};
	
	debug_say("pvgen_mpp_I start");
	debug_say(I[0]<<" "<<P[0]);
	debug_say(I[2]<<" "<<P[2]);
	debug_say(I[4]<<" "<<P[4]);
	
	while (fabs(I[0]-I[4]) > eMax) {
		I[1] = (I[0]+I[2])/2;
		P[1] = r.V(I[1])*I[1];
		I[3] = (I[2]+I[4])/2;
		P[3] = r.V(I[3])*I[3];
		
		debug_say(I[1]<<" "<<P[3]);
		debug_say(I[1]<<" "<<P[3]);
		
		if (P[1]>P[2] && P[1]>P[3]) { // P1 is closer to MPP
			I[4]=I[2]; P[4]=P[2];     //   Move 2 -> 4
			I[2]=I[1]; P[2]=P[1];     //   Move 1 -> 2
		} else if (P[2]>P[3]) {       // P2 is closer to MPP
			I[0]=I[1]; P[0]=P[1];     //   Move 1 -> 0
			I[4]=I[3]; P[4]=P[3];     //   Move 3 -> 4
		} else {                      // P3 is closer to MPP
			I[0]=I[2]; P[0]=P[2];     //   Move 2 -> 0
			I[2]=I[3]; P[2]=P[3];     //   Move 3 -> 2
		}
	}
	
	debug_say("pvgen_mpp_I end: I="<<I[2]);
	return I[2];
}

#else

typedef double (*bisect_max_eval_fcn)(double, void*);
static double bisect_max(double x0, double x1, double ex, bisect_max_eval_fcn eval, void *p) {
	double X[5] = {
		x0,
		0,
		(x0+x1)/2,
		0,
		x1
	};
	double Y[5] = {
		eval(X[0],p),
		0,
		eval(X[2],p),
		0,
		eval(X[4],p)
	};
	
	while (fabs(X[4]-X[0]) > ex) {
		X[1] = (X[0]+X[2])/2;
		Y[1] = eval(X[1],p);
		X[3] = (X[2]+X[4])/2;
		Y[3] = eval(X[3],p);
		if (Y[1]>Y[2] && Y[1]>Y[3]) { // Y1 is closer to max
			X[4]=X[2]; Y[4]=Y[2];     //   Move 2 -> 4
			X[2]=X[1]; Y[2]=Y[1];     //   Move 1 -> 2
		} else if (Y[2]>Y[3]) {       // PY is closer to max
			X[0]=X[1]; Y[0]=Y[1];     //   Move 1 -> 0
			X[4]=X[3]; Y[4]=Y[3];     //   Move 3 -> 4
		} else {                      // Y3 is closer to max
			X[0]=X[2]; Y[0]=Y[2];     //   Move 2 -> 0
			X[2]=X[3]; Y[2]=Y[3];     //   Move 3 -> 2
		}
	}
	
	return X[2];
}

static double pvgen_mpp_I_aux(double I, void *r) {
	return ((pvGenerator*)r)->V(I);
}

double pvgen_mpp_I(pvGenerator &r, double Il, double Ih, double eMax) {
	return bisect_max(Il, Ih, eMax, &pvgen_mpp_I_aux, &r);
}

#endif
