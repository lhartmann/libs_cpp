/***************************************************************************
 *   Copyright (C) 2009 by Lucas V. Hartmann <lucas.hartmann@gmail.com>    *
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

#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include "modbus.h"
#include <string>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MODBUS_TCP_PORT htons(502)
#define MODBUS_TCP_CONNECT_TIMEOUT 120000
#define MODBUS_TCP_MESSAGE_TIMEOUT 120000

namespace modbus {

/// Modbus bus connection for TCP/IP devices or MODBUS-TCP gateways.
/** Create an instance of this class if you desire to get access to
 *  Modbus devices over a TCP/IP bus. 
 * 
 *  This class contains only the specificities of MODBUS-TCP operation.
 *  All common operations are handled by the base modbus::bus class.
 */
class tcp_bus : public bus {
	int tcp_fd;
	uint8_t trans_id;
	
	/// Specialization of the modbus transaction for TCP/IP buses.
	int transaction(unsigned device, const uint8_t *obuff, int olen, uint8_t *ibuff, int *ilen, int maxilen);
	
	public:
	/// Connect to numeric IP and numeric port, with timeout.
	bool connect(uint32_t address, uint16_t port=MODBUS_TCP_PORT, uint32_t msec=MODBUS_TCP_CONNECT_TIMEOUT);
	
	/// Reconnect to the previous bus.
	bool reconnect(uint32_t msec=MODBUS_TCP_CONNECT_TIMEOUT);
	void close();
	
	/// Default constructor, bis disconnected.
	tcp_bus() : tcp_fd(-1) {}
	
	// Contructor with immediate connection.
	tcp_bus(uint32_t address, uint16_t port=MODBUS_TCP_PORT, uint32_t msec=MODBUS_TCP_CONNECT_TIMEOUT) {
		tcp_fd = -1;
		connect(address, port, msec);
	}
	~tcp_bus() {
		close();
	}
	
	operator bool () const;
};

}

#endif
