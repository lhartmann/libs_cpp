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

#ifndef MODBUS_ERRORS_H
#define MODBUS_ERRORS_H

// All modbus errors are prefixed by "MBE_"

// Protocol-defined exceptions (0x00-0xFF).
//   Found in exception responses.
#define MBE_ILLEGAL_FUNCTION            -0x001 //!< Function not implemented on device.
#define MBE_ILLEGAL_DATA_ADDRESS        -0x002 //!< Address range not implemented on device.
#define MBE_ILLEGAL_DATA_VALUE          -0x003 //!< Request fields contained bad data (should not ever happen).

// Library defined exceptions (0x100-inf).
//   Internal errors or unacceptable request detected before sending.
#define MBE_TOO_MANY                    -0x100 //!< Desired register count too large.
#define MBE_ADDRESS_OVERFLOW            -0x101 //!< Desired read range overflows the address range.
#define MBE_BAD_RESPONSE_LENGTH         -0x102 //!< Received response length differs from expected.
#define MBE_NOT_CONNECTED               -0x103 //!< Communication link to the device was not established properly.
#define MBE_NO_MEMORY                   -0x104 //!< Out of memory (unlikely to ever happen).
#define MBE_SEND_REQUEST_FAILED         -0x105 //!< Internal error sending request (write syscall failed).
#define MBE_BAD_POINTERS                -0x106 //!< Either input buffer data or length pointers were NULL.
#define MBE_RECEIVE_FAILED              -0x107 //!< Internal error reading response (select or read syscalls failed).
#define MBE_UNEXPECTED_DEVICE_RESPONDED -0x108 //!< Response received from a device different of the request.
#define MBE_RESPONSE_TOO_LONG           -0x109 //!< Response was larger than the available buffer.
#define MBE_TIMED_OUT                   -0x10A //!< No response received on time.
#define MBE_CRC_FAIL                    -0x10B //!< Response CRC16 check did not match.
#define MBE_BAD_TRANSACTION_ID          -0x10C //!< Transaction ID does not match request (MODBUS-TCP)
#define MBE_BAD_PROTOCOL_ID             -0x10D //!< Protocol ID is not zero (MODBUS-TCP)

#include "modbus_errors_by_name.h"
#endif
