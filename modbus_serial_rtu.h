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

#ifndef MODBUS_SERIAL_RTU_H
#define MODBUS_SERIAL_RTU_H

#include "modbus.h"
#include "modbus_serial.h"

namespace modbus {

/// Bus connection for devices using Modbus Serial-RTU (binary) protocol.
/** Create an instance of this class if you desire to get access to
 *  Modbus Serial-RTU devices. 
 * 
 *  This class contains only the specificities of the RTU protocol. Serial
 *  port is handled by modbus::serial_bus, and all common Modbus operations
 *  are handled by the base modbus::bus class.
 */
class rtu_serial_bus : public serial_bus {
	int transaction(unsigned device, const uint8_t *obuff, int olen, uint8_t *ibuff, int *ilen, int maxilen);
	
	public:
	rtu_serial_bus() {}
	rtu_serial_bus(const char *devname, unsigned long baud) {
		open(devname, baud);
	}
};

}

#endif
