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

// Limpar definições antigas
#ifdef debug_pre
#undef debug_pre
#endif

#ifdef debug_say
#undef debug_say
#endif

#ifdef debug_dump
#undef debug_dump
#endif

#ifdef debug_dump_vector
#undef debug_dump_vector
#endif

#ifdef debug_flag
#undef debug_flag
#endif

//Definir valores novos

#ifdef DEBUG

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdint.h>
#include <string>

extern std::ofstream debug_file;
#ifndef DEBUG_STREAM
#define DEBUG_STREAM std::cerr //debug_file
#endif

std::string debug_shorten_filename(std::string filename);

#define debug_pre() DEBUG_STREAM<<"[At line "<<__LINE__<<" of "<<debug_shorten_filename(__FILE__)<<"] ";
#define debug_dump(v) { debug_pre(); DEBUG_STREAM<< #v " is "<<(v)<<"."<<std::endl; }
#define debug_say(v) { debug_pre(); DEBUG_STREAM<<v<<std::endl; }
#define debug_flag() { debug_pre(); DEBUG_STREAM<<std::endl; }
#define debug_dump_vector(v,n,fcn) { \
	debug_pre(); \
	DEBUG_STREAM<< #v " is { "<<fcn((v)[0]); \
	for (int debi=1; debi<(n); ++debi) DEBUG_STREAM << ", " << fcn((v)[debi]); \
	DEBUG_STREAM << " }."<<std::endl; \
}
#define DEBUG_DUMP_CHAR_AS_HEX(c) "0x"<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(uint8_t(c))<<std::dec
#define DEBUG_DUMP_CHAR_AS_DEC(c) unsigned(uint8_t(c))
#define DEBUG_DUMP_INT_AS_HEX(x) "0x"<<std::hex<<std::setw(8)<<std::setfill('0')<<unsigned(x)<<std::dec

#else

#define debug_dump(v) ;
#define debug_say(v) ;
#define debug_flag(v) ;
#define debug_dump_vector(v,n,fcn) ;

#endif
