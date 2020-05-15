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

#include "modbus_serial_ascii.h"
#include "crc16.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

namespace modbus {

int ascii_serial_bus::transaction(unsigned device, const uint8_t *obuff, int olen, uint8_t *ibuff, int *ilen, int maxilen) {
	if (serial_fd < 0) return MBE_NOT_CONNECTED;
	
	uint8_t *buff = new uint8_t [olen+3];
	if (!buff) return MBE_NO_MEMORY;
	
	buff[0] = device & 0xFF;
	memcpy(buff+1, obuff, olen);
	uint16_t crc = crc16(buff, olen+1);
	buff[olen+1] = crc & 0xFF;
	buff[olen+2] = crc >> 8;
	
	int err = write(serial_fd, buff, olen+3);
	delete [] buff;
	if (err == -1) {
		return MBE_SEND_REQUEST_FAILED;
	}
	if (err != olen+3) {
		return MBE_SEND_REQUEST_FAILED;
	}
	
	// Expect response for unicast messages
	if (device & 0xFF) {
		// Full response must be received within 1s from the request.
		// Once the first byte is received a pause of 1ms is assumed as the end-of-response.
		fd_set rfds;
		struct timeval tv, tvnow;
		
		if (!ilen || !ibuff) return MBE_BAD_POINTERS; // What the hell?
		
		// Full timeout
		gettimeofday(&tvnow,0);
		unsigned long long now = tvnow.tv_sec * 1000000 + tvnow.tv_usec;
		unsigned long long end = now + 200000;
		
		*ilen = 0;
		
		while (now<end) {
			FD_ZERO(&rfds);
			FD_SET(serial_fd,&rfds);
			tv.tv_sec  = 0;
			tv.tv_usec = 1700;
			
			int r = select(serial_fd+1, &rfds, 0, 0, &tv);
			if (r<0) { // Error on select
				// Need: Check for SIGINT and ignore.
				if (errno != EINTR)
					return MBE_RECEIVE_FAILED;
			} else if (!r) { // Timeout
				if (*ilen > 3) { // If data was received...
					// Validate
					crc = ibuff[*ilen-1]*0x100 + ibuff[*ilen-2];
					if (crc16(ibuff,*ilen-2) != crc) return MBE_CRC_FAIL;
					
					if (ibuff[0] != device) return MBE_UNEXPECTED_DEVICE_RESPONDED;
					
					// Discard device address and crc from response.
					for (int i=1; i < *ilen; ++i) ibuff[i-1] = ibuff[i];
					*ilen -= 3;
					
					return 0; // ... operation is complete.
				}
			} else { // Data is available on serial_fd
				if (*ilen == maxilen || *ilen < 0) { // Overflow
					*ilen = -1;
					int i = read(serial_fd, ibuff, maxilen); // Discard data
					if (i<0) {
						// Error
						return MBE_RESPONSE_TOO_LONG;
					}
				} else { // Normal read operation
					int i = read(serial_fd, ibuff+*ilen, maxilen-*ilen);
					if (i<0) {
						// Error
						return MBE_RECEIVE_FAILED;
					}
					*ilen += i;
				}
			}
			
			gettimeofday(&tvnow,0);
			now = tvnow.tv_sec * 1000000 + tvnow.tv_usec;
		}
	}
	
	return MBE_TIMED_OUT; // Full timeout
}

} // namespace modbus
