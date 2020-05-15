/***************************************************************************
 *   Copyright (C) 2007 by Lucas V. Hartmann <lucas.hartmann@gmail.com>.   *
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

#include "progressbar.h"
#include <curses.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>

const uint32_t progressBar::NPOS = ~uint32_t(0);

// Get time of day as a high-res(1us) double.
static double getTime() {
	struct timeval tv;
	gettimeofday(&tv,0);
	return tv.tv_sec + 1e-6*tv.tv_usec;
}

static unsigned long detect_screen_width(int fd = STDERR_FILENO) {
#ifndef TIOCGWINSZ
	return 80;
#else
	struct winsize wsz;

	if (ioctl (fd, TIOCGWINSZ, &wsz) < 0) return 80;

	return wsz.ws_col;
#endif
}

// Constructor 1, from the total number of points
progressBar::progressBar(uint32_t nt) :
	msg("") // No message
{
		reset(nt);
}

// Constructor 2, from a message and the total number of points
progressBar::progressBar(std::string m, uint32_t nt) :
	msg(m)
{
	reset(nt);
}


void progressBar::reset() {
	reset(total);
}

void progressBar::reset(uint32_t nt) {
	total = nt;
	lastUpdated = NPOS;
	tsStart = 0;
	current = 0;
}

// Display the progressbar in the ostream (expected to be a terminal)
std::ostream &progressBar::display(std::ostream &out) {
	if (!tsStart || lastUpdated==NPOS) tsStart = getTime();

	// Check if udpate is required
	double tsNow=getTime()-tsStart;
	if (
		lastUpdated==NPOS ||
		(current-lastUpdated)*1000ULL/total!=0 ||
		current==total ||
		tsNow-tsLastUpdated>=0.5 // on firs use tsLastUpdated is uninitialized, but since lastUpdated is NPOS, this is no problem.
	) {
		lastUpdated   = current;
		tsLastUpdated = tsNow;

		screenWidth   = detect_screen_width(
			&out == &std::cout ? STDOUT_FILENO :
			&out == &std::cerr ? STDERR_FILENO :
			&out == &std::clog ? STDERR_FILENO :
			-1
		);

		// Cursor to the beggining of the line
		out<<"\x1B[1G";
		// Display the message
		out<<msg;

		// Display the bar
		uint32_t barWidth = screenWidth-msg.length()-31;
		out<<'[';
		uint32_t step=uint32_t(current*double(barWidth)/total+0.5);
		for (uint32_t i=0; i<step; ++i) out<<'=';
		for (uint32_t i=step; i<barWidth; ++i) out<<' ';
		out<<"] ";

		// Display the %
		step=int(current*1000./(total)+0.5);
		out<<std::setw(3)<<std::setfill(' ')<<(step/10)<<'.';
		out<<(step/1   %10);
		out<<"% ";

		// Elapsed time
		out                                 <<(int(tsNow/3600)   )<<'h';
		out<<std::setw(2)<<std::setfill('0')<<(int(tsNow/60  )%60)<<'m';
		out<<std::setw(2)<<std::setfill('0')<<(int(tsNow/1   )%60)<<'s';

		// Estimated total time
		out<<" of ";
		double tsEnd=tsNow*total/(current?current:1);
		out                                 <<(int(tsEnd/3600)   )<<'h';
		out<<std::setw(2)<<std::setfill('0')<<(int(tsEnd/60  )%60)<<'m';
		out<<std::setw(2)<<std::setfill('0')<<(int(tsEnd/1   )%60)<<'s';

		out<<"\x1B[K"<<std::flush;
	}
	return out;
}
