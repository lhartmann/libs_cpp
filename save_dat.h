/***************************************************************************
 *   Copyright (C) 2010 by Lucas V. Hartmann <lucas.hartmann@gmail.com>    *
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

#ifndef SAVE_DAT_H
#define SAVE_DAT_H

#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "straux.h"

/** @brief Saves a data table in a DAT file compatible with PSIM's simview.
 *  
 *  @param out  : is the output stream to write to.
 *  @param data : is the dataset as data["name"] = { values }.
 * 
 *  @returns : true on error, false if all is well.
 */
bool save_dat(std::ostream &out, std::map<std::string, std::vector<double> > data);

/** @brief Saves a data table in a DAT file compatible with PSIM's simview.
 *  
 *  @param filename : is the output file name, it will be created and truncted.
 *  @param data     : is the dataset as data["name"] = { values }.
 * 
 *  @returns : true on error, false if all is well.
 */
inline bool save_dat(std::string filename, std::map<std::string, std::vector<double> > data) {
	using namespace std;
	ofstream out(filename.c_str(), ios::out|ios::trunc);
	return save_dat(out, data);
}

#endif
