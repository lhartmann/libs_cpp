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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "modbus.h"

//#define DEBUG
#include "debug.h"

namespace modbus {

void modbus_error::output(std::ostream &out) {
	out<<"Error at line "<<line<<" of "<<file<<", while "<<message<<", device "<<device<<", address "<<address<<", error code "<<code<<"."<<std::endl;
}

////////////////////////////
// 16-bit register access //
////////////////////////////

// Function code 0x10
int bus::writeHoldingRegisters(unsigned device, uint16_t address, uint16_t count, const uint16_t *data) {
// Request: code[1], address[2], count[2], bytes[1], data[bytes]
// Response: code[1], address[2], count[2]
	// Sanity Check
	if (count > 123) return MBE_TOO_MANY;
	if (address+count < address) return MBE_ADDRESS_OVERFLOW;
	
	// Build request
	uint8_t buff[258], *p=buff;
	*p++ = 0x10;
	*p++ = (address >> 8) & 0xFF;
	*p++ = (address     ) & 0xFF;
	*p++ = (count   >> 8) & 0xFF;
	*p++ = (count       ) & 0xFF;
	*p++ = (count * 2   ) & 0xFF;
	for (int i=0; i<count; ++i) {
		*p++ = (data[i] >> 8) & 0xFF;
		*p++ = (data[i]     ) & 0xFF;
	}
	
	// Run transaction
	int ilen;
	int j = transaction(device, buff, p-buff, buff, &ilen, 258);
	if (j) return j; // Transaction error
	
	// Validate response
	if (buff[0] & 0x80) { // Exception
		return -buff[1];
	}
	return 0;
}

// 0x03
int bus::readHoldingRegisters (unsigned device, uint16_t address, uint16_t count, uint16_t *data) {
// Request: code[1], address[2], count[2]
// Response: code[1], bytes[1], data[bytes]
	// Sanity Check
	if (count > 123) return MBE_TOO_MANY;
	if (address+count < address) return MBE_ADDRESS_OVERFLOW; // overflow
	
	// Build request
	uint8_t buff[258], *p=buff;
	*p++ = 0x03;
	*p++ = (address >> 8) & 0xFF;
	*p++ = (address     ) & 0xFF;
	*p++ = (count   >> 8) & 0xFF;
	*p++ = (count       ) & 0xFF;
	
	// Run transaction
	int ilen;
	int j = transaction(device, buff, p-buff, buff, &ilen, 258);
	if (j) return j; // Transaction error
	
	// Validate response
	p=buff;
	if (*p++ & 0x80) return -*p; // Exception
	if (*p++ != count*2) return MBE_BAD_RESPONSE_LENGTH; // Odd number of bytes in response
	if (ilen != count*2+2) return MBE_BAD_RESPONSE_LENGTH; // Odd number of bytes in response
	
	// Everything is right, read data
	for (int i=0; i<count; ++i) {
		uint16_t u = *p++;
		data[i] = (u<<8) | *p++;
	}
	return 0;
}

// Function Code 0x04
int bus::readInputRegisters   (unsigned device, uint16_t address, uint16_t count, uint16_t *data) {
// Request: code[1], address[2], count[2]
// Response: code[1], bytes[1], data[bytes]
	// Sanity Check
	if (count > 123) return MBE_TOO_MANY;
	if (address+count < address) return MBE_ADDRESS_OVERFLOW; // overflow
	
	// Build request
	uint8_t buff[258], *p=buff;
	*p++ = 0x04;
	*p++ = (address >> 8) & 0xFF;
	*p++ = (address     ) & 0xFF;
	*p++ = (count   >> 8) & 0xFF;
	*p++ = (count       ) & 0xFF;
	
	// Run transaction
	int ilen;
	int j = transaction(device, buff, p-buff, buff, &ilen, 258);
	if (j) return j; // Transaction error
	
	// Validate response
	p=buff;
	if (*p++ & 0x80) return -*p; // Exception
	if (*p++ != count*2) return MBE_BAD_RESPONSE_LENGTH; // Odd number of bytes in response
	if (ilen != count*2+2) return MBE_BAD_RESPONSE_LENGTH; // Odd number of bytes in response
	
	// Everything is right, read data
	for (int i=0; i<count; ++i) {
		uint16_t u = *p++;
		data[i] = (u<<8) | *p++;
	}
	return 0;
}

// 1-bit register access

// 0x0F
int bus::writeCoils       (unsigned device, uint16_t address, uint16_t count, const uint8_t *data) {
// Request: code[1], addr[2], count[2], bytes[1], data[bytes]
// Response: code[1], address[2],count[2]
	return -999;
}

// 0x01
int bus::readCoils        (unsigned device, uint16_t address, uint16_t count, uint8_t *data) {
// Request: code[1], addr[2], count[2]
// Answer: code[], bytes[1], data[bytes]
	return -999;
}

// 0x02
int bus::readInputDiscrete(unsigned device, uint16_t address, uint16_t count, uint8_t *data) {
// Request: code[1], addr[2], count[2]
// Answer: code[1], bytes[1], data[bytes]
	return -999;
}

void wordswap(uint32_t *data, uint32_t n) {
	for (uint32_t i=0; i<n; ++i) data[i] = (data[i]>>16) | (data[i]<<16);
}

}

