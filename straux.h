/***************************************************************************
 *   Copyright (C) 2016 Lucas V. Hartmann <lucas.hartmann@gmail.com>       *
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

#ifndef STRAUX_H
#define STRAUX_H

#include <string>
#include <vector>
#include <cstdlib>

// Splits a string int a vector using div as the separator.
std::vector<std::string> strSplit(std::string str, char div=';');

// Returns true if the string is a valid floating point value.
bool strIsFloat(std::string str);

// Changes string into double. Returns NAN on error.
double strToFloat(std::string str);

// Changes a floating point value into a decimal string with given precision.
std::string floatToString(double d, int precision=15);

// Changes a dacimal string into a floating point value.
inline double strToFloat(std::string s) { return atof(s.c_str()); }

// Returns true if the string is a valid decimal-encoded integger value.
bool strIsInt(std::string str);

// Changes an integger value to a string, with at least digits, using base.
std::string intToString(long int i, int digits=1, int base=10);

// Changes a string using decimal encoding into an integger.
unsigned long strToInt(std::string s, int base=10);

// Returns true if the given string is a valid SRT-style timestamp
//   e.g.: 01:23:45.678
bool strIsTimestamp(std::string s);

// Changes a SRT-style timestampe into a time in seconds.
double strToTimestamp(std::string s);

// Changes a time in senconds into a SRT-style timestamp
std::string timestampToString(double d);

// Removes blanks (i.e. " \t\f\b\r\n") from beginning and end of string.
std::string strClean(std::string s);

// Escape C's special chars from string.
//   If unix_style if true then shell special chars are escaped as well.
std::string strEscape(std::string s, bool unix_style=false);

// Unescape C's special chars from string.
//   If unix_style if true then shell special chars are unescaped as well.
std::string strUnescape(std::string s, bool unix_style=false);

#endif
