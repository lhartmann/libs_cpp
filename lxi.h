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

#ifndef LXIDEV_H
#define LXIDEV_H

#include <string>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LXI_DEFAULT_PORT htons(5025)
#define LXI_DEFAULT_CONECT_TIMEOUT 120000
#define LXI_DEFAULT_MESSAGE_TIMEOUT 120000

// Connect to numeric IP and numeric port, with timeout.
int lxiConnectWithTimeout(uint32_t msec, uint32_t address, uint16_t port=LXI_DEFAULT_PORT);

// Look for idn in *IDN response of devices in the range from ip0 to ip1.
uint32_t lxiScanRange(std::string idn, uint32_t ip0, uint32_t ip1, uint16_t port=LXI_DEFAULT_PORT);

// lxiMessage
bool lxiMessageWithTimeout(int fd, std::string cmd, std::string *answer, uint32_t msec);

// Shortcuts for the functions above

// Connect to textual addresss [and numeric port]
inline int lxiConnectWithTimeout(uint32_t msec, std::string address, uint16_t port=LXI_DEFAULT_PORT) {
	// Could add dns resolution here
	uint32_t ip = inet_addr(address.c_str());
	return lxiConnectWithTimeout(msec, ip, port);
}

// Connect to numeric IP [and numeric port], with default timeout
inline int lxiConnect(uint32_t address, uint16_t port=LXI_DEFAULT_PORT) {
	return lxiConnectWithTimeout(LXI_DEFAULT_CONECT_TIMEOUT,address,port);
}

// Connect to textual addresss [and numeric port], with default timeout
inline int lxiConnect(std::string address, uint16_t port=LXI_DEFAULT_PORT) {
	return lxiConnectWithTimeout(LXI_DEFAULT_CONECT_TIMEOUT,address, port);
}

inline bool lxiMessage(int fd, std::string cmd, std::string *answer) {
	return lxiMessageWithTimeout(fd, cmd, answer, LXI_DEFAULT_MESSAGE_TIMEOUT);
}

inline std::string lxiMessage(int fd, std::string cmd, int to=LXI_DEFAULT_MESSAGE_TIMEOUT) {
	std::string s, *p = &s;
	if (cmd.length()<1) p=0;
	else if (cmd[cmd.length()-1]!='?') p=0;
	if (lxiMessageWithTimeout(fd, cmd, p, to)) return "TIMED_OUT";
	return s;
}

// Get identification from opened instrument
inline std::string lxiIdentify(int fd, int to=LXI_DEFAULT_MESSAGE_TIMEOUT) { return lxiMessage(fd,"*IDN?",to); }

#endif
