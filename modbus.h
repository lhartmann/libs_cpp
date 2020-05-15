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

#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>
#include "error.h"
#include "modbus_errors.h"

namespace modbus {

/** Generic modbus error class, thrown by several functions.
 */
class modbus_error : public error {
	uint16_t device;  //!< Modbus device that generated the error.
	uint16_t address; //!< Address being accessed, if any.
	int code;         //!< Error code, @see modbus_errors.h
	
	/// Output user-friendly error message.
	void output(std::ostream &out);
	
	public:
	// Simple constructor
	modbus_error(int l, std::string f, std::string msg, uint16_t dev, uint16_t addr, int c) : error(l,f,msg), device(dev), address(addr), code(c) {}
};

class bus;
class device;
class holdingRegister;

/** Pure virtual base class for generic bus access.
 * 
 * This class should only be used as pointer type to hold real buffer
 * references, or as a base class when creating a new bus connection.
 */
class bus {
	friend class device;
	protected:
	
	/// Reference counter, for bus locking.
	unsigned refcount; 

	/// Lock the bus, increase reference counter.
	void lock() {
		refcount++;
	}

	/// Release the bus.
	/** Decreases reference counter. May be replaced by derivative classes to
	 * close bus connection once it is no longer necessary.
	 */
	virtual void release() {
		if (refcount) refcount--;
	}
	
	/// Runs a single transaction.
	/** Buffers hold PDUs only. ADU is to be added depending on the underlying
	 *  bus architecture.
	 */
	virtual int transaction(unsigned device, const uint8_t *obuff, int olen, uint8_t *ibuff, int *ilen, int maxilen) = 0;
	
	public:
	virtual ~bus() {};
	
	/// Get a handler for a specific device.
	device operator[] (unsigned addr);
	
	/// Returns true if the bus is ready for use.
	virtual operator bool () const = 0;

	/// Write 16-bit holding register.
	int writeHoldingRegisters(unsigned device, uint16_t address, uint16_t count, const uint16_t *data);
	
	/// Read 16-bit holding register.
	int readHoldingRegisters (unsigned device, uint16_t address, uint16_t count, uint16_t *data);

	/// Read 16-bit input register (read-only).
	int readInputRegisters   (unsigned device, uint16_t address, uint16_t count, uint16_t *data);
	
	/// Writes 1-bit coil registers.
	int writeCoils       (unsigned device, uint16_t address, uint16_t count, const uint8_t *data);

	/// Reads 1-bit coil registers.
	int readCoils        (unsigned device, uint16_t address, uint16_t count, uint8_t *data);

	/// Reads 1-bit input registers (read-only).
	int readInputDiscrete(unsigned device, uint16_t address, uint16_t count, uint8_t *data);
};

/// Modbus device handler class.
/** This class may be used to get access to a modbus device on any modbus connection.
 */
class device {
	friend class bus;
	
	/// Bus to which the device is connected.
	bus *b;
	
	// Device's address in the bus it is connected to.
	unsigned d;
	
	/// Contructor, may only be accessed via modbus::bus[address].
	device(bus *nb, unsigned nd) : b(nb), d(nd) { b->lock(); }
	
	public:
	/// Default constructor, allows uninitialized/disconnected devices.
	device() : b(0), d(0) {}
	
	/// Copy constructor, allow passing a device as an argument to a function.
	device(const device &src) {
		b = src.b;
		d = src.d;
		if (b) b->lock();
	}
	
	~device() { if (b) b->release(); }
	
	/// Returns true if the device is ready for use.
	operator bool() {
		if (!b) return false;
		return *b;
	}
	
	/// Copy device.
	const device &operator= (const device &src) {
		if (b) b->release();
		b = src.b;
		d = src.d;
		if (b) b->lock();
		return *this;
	}
	
	// Disconnect device from bus. Device becomes uninitialized.
	void release() {
		if (b) b->release();
		b=0;
		d=0;
	}
	
	/// Returns the address of the device in the bus.
	unsigned getAddress() const { return d; }
	
	/// Write to 16-bit holding registers.
	int writeHoldingRegisters (uint16_t address, uint16_t count, const uint16_t *data) {
		if (!b) return MBE_NOT_CONNECTED;
		return b->writeHoldingRegisters (d, address, count, data);
	}
	
	/// Read 16-bit holding registes.
	int readHoldingRegisters (uint16_t address, uint16_t count, uint16_t *data) {
		if (!b) return MBE_NOT_CONNECTED;
		return b->readHoldingRegisters (d, address, count, data);
	}
	
	// Read 16-bit inout registers (read-only).
	int readInputRegisters (uint16_t address, uint16_t count, uint16_t *data) {
		if (!b) return MBE_NOT_CONNECTED;
		return b->readInputRegisters (d, address, count, data);
	}
	
	// Write 1-bit coil registers.
	int writeCoils (uint16_t address, uint16_t count, const uint8_t *data) {
		if (!b) return MBE_NOT_CONNECTED;
		return b->writeCoils (d, address, count, data);
	}

	// Read 1-bit coil registers.
	int readCoils (uint16_t address, uint16_t count, uint8_t *data) {
		if (!b) return MBE_NOT_CONNECTED;
		return b->readCoils (d, address, count, data);
	}
	
	// Read 1-bit input registers (read-only).
	int readInputDiscrete (uint16_t address, uint16_t count, uint8_t *data) {
		if (!b) return MBE_NOT_CONNECTED;
		return b->readInputDiscrete (d, address, count, data);
	}
	
	/// Gets a handle for a single holding register (read-write).
	inline holdingRegister operator[] (unsigned addr);
	
	/// Gets the value of a single input register (read-only).
	uint16_t operator() (unsigned addr) {
		uint16_t u;
		int err = readInputRegisters(addr, 1, &u);
		if (err) {
			throw modbus_error(__LINE__, __FILE__, "reading input register", d, addr, err);
		}
		return u;
	}
};

/// Handle class for holding registers (read-write).
class holdingRegister {
	friend class device;
	device *dev;
	unsigned addr;
	
	/// Constructor, private, get a handle from modbus::device[reg].
	holdingRegister(device *d, unsigned a) : dev(d), addr(a) {};
	public:
	
	/// Write a value to the holding register.
	uint16_t operator = (uint16_t u) {
		int err = dev->writeHoldingRegisters(addr, 1, &u);
		if (err) {
			throw modbus_error(__LINE__, __FILE__, "writing holding register", dev->getAddress(), addr, err);
		}
		return u;
	}
	
	/// Reads a value from a holding register.
	operator uint16_t () {
		uint16_t u;
		int err = dev->readHoldingRegisters(addr, 1, &u);
		if (err) {
			throw modbus_error(__LINE__, __FILE__, "reading holding register", dev->getAddress(), addr, err);
		}
		return u;
	}
};

inline device bus::operator[] (unsigned addr) {
	return device(this, addr);
}

inline holdingRegister device::operator[] (unsigned addr) {
	return holdingRegister(this, addr);
}

/// Accessory function to perform 16-bit word swaps.
void wordswap(uint32_t *data, uint32_t n);

} // namespace

#endif

