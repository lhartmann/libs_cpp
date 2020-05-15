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

#ifndef DXF_H
#define DXF_H

#include <fstream>
#include <string>

class DxfExporter {
	std::ofstream out;
	unsigned count;
	
public: 
	DxfExporter();
	DxfExporter(std::string fname);
	~DxfExporter();
	
	bool open(std::string fname);
	void close();
	
	void line(double x0, double y0, double z0, double x1, double y1, double z1);
	void line(double x0, double y0, double x1, double y1) {
		line(x0,y0,0,  x1,y1,0);
	}
	
	void circle(double x0, double y0, double z0, double r);
	void circle(double x0, double y0, double r) {
		circle(x0,y0,0,r);
		
	}
	
	void ellipse(double x0, double x1, double y0, double y1, double thick);
};


#endif
