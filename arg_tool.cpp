/***************************************************************************
 *   Copyright (C) 2006 by Lucas V. Hartmann <lucas.hartmann@gmail.com>.   *
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

#include "arg_tool.h"
#include <string.h>

// #define DEBUG
#include "debug.h"

int arg_eval(int argc, const char *argv[], const arg_t *args) {
	// Set all indexes to zero
	for (int i=0; args[i].id; ++i) {
		if (!args[i].index) {
			debug_say("Bad index address for "<<args[i].id<<".");
			return -(i+1);
		}
		*args[i].index = 0;
	}
	
	// For every command line argument, try matching against the expected table
	for (int arg=1; arg<argc; ++arg) {
		// Search for argument in the table
		int i=0;
		while (args[i].id) {
			if (!strcmp(argv[arg],args[i].id)) break;
			++i;
		}
		
		// Check error: Not found.
		if (!args[i].id) {
			debug_say("Argument "<<argv[arg]<<" is not recognized.");
			return arg;
		}
		
		// Check error: Duplicated on the command line.
		if (*args[i].index) {
			debug_say("Argument "<<argv[arg]<<" is duplicated.");
			return arg; 
		}
		
		// End of argument list, stop parsing
		if (args[i].flags & ARG_END) {
			*args[i].index = arg+1;
			break;
		}
		
		// Flag type, no parameters required.
		if ((args[i].flags & ARG_TYPE) == ARG_FLAG) {
			// Just set index to 1, true.
			*args[i].index = 1;
		}
		
		// Default type, expects a parameter.
		//   E.G.: --output file.txt
		if ((args[i].flags & ARG_TYPE) == ARG_DEFAULT) {
			// Check error: missing parameter.
			if (arg+1 >= argc) {
				debug_say("Argument "<<argv[arg]<<" requires a parameter, but none was given.");
				return arg;
			}
			
			// Set index to the position of the parameter and discard it.
			*args[i].index = ++arg;
		}
	}
	
	// Check for missing required arguments.
	for (int i=0; args[i].id; ++i) {
		if (!*args[i].index && (args[i].flags & ARG_REQUIRED)) {
			debug_say("Argument "<<args[i].id<<" is required, but was not specified.");
			return -(i+1);
		}
	}
	
	return 0;
}
