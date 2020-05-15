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

#ifndef MODBUS_SERIAL_H
#define MODBUS_SERIAL_H

#include "modbus.h"

namespace modbus {

/// Base class providing serial port functionality for MODBUS buses.
/** This is a pure-virtual class, and you should never need to use it
 *  directly. Use modbus::bus for generic pointers, and either
 *  modbus::rtu_serial_bus or modbus::ascii_serial_bus depending on the type
 *  of serial bus you desire.
 */
class serial_bus : public bus {
protected:
	int serial_fd;
	
public:
	/// Connect to a serial port.
	/** @param devname  Serial port device. E.g.: /dev/ttyUSB0
	 *  @param baud     Serial port baud rate. E.g.: 115200
	 */
	void open(const char *devname, unsigned long baud);

	/// Disconnect from the serial port.
	void close();
	
	serial_bus() : serial_fd(-1) {}
	serial_bus(const char *devname, unsigned long baud) {
		serial_fd = -1;
		open(devname, baud);
	}
	~serial_bus() {
		close();
	}
	
	operator bool () const;
};

}

#endif
