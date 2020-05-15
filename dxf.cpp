/***************************************************************************
 *   Copyright (C) 2014 by Lucas V. Hartmann <lucas.hartmann@gmail.com>    *
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

#include "dxf.h"
#include <cmath>

using namespace std;

DxfExporter::DxfExporter() {

}

DxfExporter::~DxfExporter() {
	close();
}

DxfExporter::DxfExporter(std::string fname) {
	open(fname);
}

bool DxfExporter::open(std::string fname) {
	close();
	
	out.open(fname.c_str(), ios::out | ios::trunc);
	count = 1;
	
	out << 0 << endl;
	out << "SECTION" << endl;
	out << 2 << endl;
	out << "ENTITIES" << endl;
	
    return bool(out);
}

void DxfExporter::close() {
	if (out.is_open()) {
		out << 0 << endl;
		out << "ENDSEC" << endl;
		out << 0 << endl;
		out << "EOF" << endl;
	}
	out.close();
}

void DxfExporter::line(double x0, double y0, double z0, double x1, double y1, double z1) {
	out << "0" << endl;
	out << "LINE" << endl;
	out << "8" << endl;
	out << "0" << endl;
	out << "10" << endl;
	out << x0 << endl;
	out << "20" << endl;
	out << y0 << endl;
	out << "30" << endl;
	out << z0 << endl;
	out << "11" << endl;
	out << x1 << endl;
	out << "21" << endl;
	out << y1 << endl;
	out << "31" << endl;
	out << z1 << endl;
}

void DxfExporter::circle(double x0, double y0, double z0, double r) {
	out << "0" << endl;
	out << "CIRCLE" << endl;
	out << "8" << endl;
	out << "0" << endl;
	out << "10" << endl;
	out << x0 << endl;
	out << "20" << endl;
    out << y0 << endl;
	out << "30" << endl;
	out << z0 << endl;
	out << "40" << endl;
	out << r << endl;
}

void DxfExporter::ellipse(double x0, double x1, double y0, double y1, double rp) {
	double z0 = 0;
	double z1 = 0;
	double start_angle = 0;
	double stop_angle = 2*M_PI;
	out << "0" << endl;
	out << "ELLIPSE" << endl;
	out << "8" << endl; // Layer
	out << "0" << endl;
	out << " 10" << endl; // X0
	out << x0 << endl;
	out << " 20" << endl; // Y0
	out << y0 << endl;
	out << " 30" << endl; // Z0
	out << z0 << endl;
	out << "11" << endl; // X1
	out << x1 << endl;
	out << "21" << endl; // Y1
	out << y1 << endl;
	out << "31" << endl; // Z1
	out << z1 << endl;
	out << "210" << endl; // Surface Normal X
	out << "0.0" << endl;
	out << "220" << endl; // Surface Normal Y
	out << "0.0" << endl;
	out << "230" << endl; // surface Normal Z
	out << "1.0" << endl;
	out << "40" << endl; // Scale factor over axis
	double sf = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0)) / 2 / rp;
	out << sf << endl;
	out << "41" << endl;
	out << start_angle << endl;
	out << "42" << endl;
	out << stop_angle << endl;
}




