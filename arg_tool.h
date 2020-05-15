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

#ifndef ARG_TOOL_H
#define ARG_TOOL_H

struct arg_t {
	const char *id;
	int *index;
	int flags;
};

#define ARG_TYPE        0x0F // Bitmask for flag type
#define ARG_DEFAULT		0x01
#define ARG_FLAG		0x02 // FLAG argument, don't expect any value.
#define ARG_REQUIRED	0x10 // Argument is mandatory
#define ARG_END         0x80 // Stop parsing at this point

extern int arg_eval(int argc, const char *argv[], const arg_t *args);

#endif
