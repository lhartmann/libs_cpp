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

#include "modbus_tcp.h"
#include "modbus_errors.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

//#define DEBUG
#include <debug.h>

namespace modbus {

int tcp_bus::transaction(unsigned device, const uint8_t *obuff, int olen, uint8_t *ibuff, int *ilen, int maxilen) {
	if (tcp_fd < 0) return MBE_NOT_CONNECTED;
	
	uint8_t *buffer = new uint8_t[olen+7];
	if (!buffer) return MBE_NO_MEMORY;
	++trans_id;
	buffer[0] = 0xBE; // Use part of transaction id as magic number
	buffer[1] = trans_id;
	buffer[2] = 0;
	buffer[3] = 0;
	buffer[4] = (olen+1) >> 8;
	buffer[5] = (olen+1) & 0xFF;
	buffer[6] = device & 0xFF;
	memcpy(buffer+7, obuff, olen);
	
	debug_say("Requesting from device "<<device<<" via fd "<<tcp_fd<<"...");
	if (write(tcp_fd, buffer, olen+7) != olen+7) {
		delete [] buffer;
		return MBE_SEND_REQUEST_FAILED;
	}
	debug_say("  Request sent.");
	delete [] buffer;
	
	// Expect no response for bradcast messages
	if (device & 0xFF == 0) return 0;
	debug_say("  Expecting response...");
	
	// Timeout parameters
	int msec=120000;
	
	fd_set rtcp_fds, etcp_fds;
	FD_ZERO(&rtcp_fds);
	FD_SET(tcp_fd,&rtcp_fds);
	FD_ZERO(&etcp_fds);
	FD_SET(tcp_fd,&etcp_fds);
	struct timeval tv, end, now;
	tv.tv_sec  =  msec/1000;
	tv.tv_usec = (msec%1000)*1000;
	
	// Wait for data
	gettimeofday(&end,0);
	end.tv_sec  += tv.tv_sec;
	end.tv_usec += tv.tv_usec;
	if (end.tv_usec > 1000000) {
		end.tv_usec -= 1000000;
		end.tv_sec ++;
	}
	int rcvd=0, rsz=-1;
	int len = maxilen+10;
	buffer = new uint8_t [len];
	if (!buffer) return MBE_NO_MEMORY;
	
	debug_say("  Receive loop...");
	while (1) {
		if (select(tcp_fd+1, &rtcp_fds, 0, &etcp_fds, &tv) != 1) {
			// either select failed or connection timed out
			delete [] buffer;
			return MBE_RECEIVE_FAILED;
		}
		
		int i = read(tcp_fd, buffer+rcvd, len-rcvd);
		debug_say("    read returned "<<i);
		if (i<1) {
			delete [] buffer;
			return MBE_RECEIVE_FAILED;
		}
		debug_dump_vector(buffer+rcvd, i, DEBUG_DUMP_CHAR_AS_HEX);
		
		// First byt must be 0xBE as i sent it in the transaction ID.
		if (rcvd==0) {
			int off;
			for (off=0; off<i; ++i) {
				if (buffer[off]==0xBE) break;
			}
			if (off) debug_say("    dropping "<<off<<" bytes.");
			for (int j=off; j<i; ++j) {
				buffer[j-off] = buffer[j];
			}
			i -= off;
		}
		
		rcvd += i;
		
		debug_say("    rcvd="<<rcvd);
		
		if (rsz<0 && rcvd>=7) {
			rsz = buffer[4]*256 + buffer[5]; // expected response size
			debug_say("    rsz="<<rsz);
		}
		if (rsz>=0 && rcvd == rsz+6) break;
		
		gettimeofday(&now,0);
		if (now.tv_sec > end.tv_sec) {
			delete [] buffer;
			return MBE_TIMED_OUT;
		}
		if (now.tv_sec < end.tv_sec) continue;
		if (now.tv_usec > end.tv_usec) {
			delete [] buffer;
			return MBE_TIMED_OUT;
		}
	}
	if (buffer[6] != device & 0xFF) {
		delete [] buffer;
		return MBE_UNEXPECTED_DEVICE_RESPONDED;
	}
	if (buffer[1] != trans_id) {
		delete [] buffer;
		return MBE_BAD_TRANSACTION_ID;
	}
	if (buffer[2] != 0 || buffer[3] != 0) {
		delete [] buffer;
		return MBE_BAD_PROTOCOL_ID;
	}
	
	memcpy(ibuff, buffer+7, rsz-1);
	*ilen = rsz-1;
	delete [] buffer;
	
	return 0;
}

bool tcp_bus::connect(uint32_t address, uint16_t port, uint32_t msec) {
	if (tcp_fd != -1) return true;
	debug_say("Connecting...");
	
	trans_id = 0;
	
	debug_say("  socket(AF_INET,SOCK_STREAM,0)");
	int fd = socket(AF_INET,SOCK_STREAM,0);
	if (fd==-1) return true;
	
	// Get control flags
	debug_say("  fcntl(fd,F_GETFL,0)");
	int flags = fcntl(fd,F_GETFL,0);
	if (flags == -1) {
		debug_say("    Returned with errno "<<errno);
		::close(fd);
		return true;
	}
	
	// Set non-blocking mode
	debug_say("  fcntl(fd,F_SETFL,flags|O_NONBLOCK)");
	if (fcntl(fd,F_SETFL,flags|O_NONBLOCK)==-1) {
		debug_say("    Returned with errno "<<errno);
		::close(fd);
		return true;
	} 
	
	// Connection parameters
	struct sockaddr_in sa;
	memset(&sa,0,sizeof(sa));
	sa.sin_family      = AF_INET;
	sa.sin_port        = port;
	sa.sin_addr.s_addr = address;
	
	// Timeout parameters
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd,&fds);
	struct timeval tv;
	tv.tv_sec  =  msec/1000;
	tv.tv_usec = (msec%1000)*1000;
	
	// Request connection
	debug_say("  ::connect(fd, (struct sockaddr *)&sa, sizeof(sa))");
	if (::connect(fd, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
		if (errno!=EINPROGRESS && errno!=EAGAIN && errno!=EWOULDBLOCK) {
			debug_say("    Returned with errno "<<errno);
			::close(fd);
			return true;
		}
	}
	debug_say("  select(fd+1, 0, &fds, 0, &tv)");
	if (select(fd+1, 0, &fds, 0, &tv) != 1) {
		// either select failed or connection timed out
		debug_say("    Returned with errno "<<errno);
		::close(fd);
		return true;
	} 
	
	// Return to blocking mode
	debug_say("  fcntl(fd,F_SETFL,flags&~O_NONBLOCK)");
	if (fcntl(fd,F_SETFL,flags&~O_NONBLOCK)==-1) {
		debug_say("    Returned with errno "<<errno);
		::close(fd);
		return true;
	}
	
	// Disable Nagle's algorithm (for reduced latency)
	int nd = 1;
	debug_say("  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nd, sizeof(nd))");
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nd, sizeof(nd))==-1) {
		debug_say("    Returned with errno "<<errno);
		::close(fd);
		return true;
	}
	
	// Everything went fine.
	debug_say("  Connection successfull.");
	tcp_fd = fd;
	return false;
}

bool tcp_bus::reconnect(uint32_t msec) {
	if (tcp_fd == -1) return true;
	debug_say("Reconnecting...");
	
	// Discover previous address
	sockaddr_in sa;
	socklen_t sal = sizeof(sa);
	if (getpeername(tcp_fd, (sockaddr*)&sa, &sal)) {
		return true;
	}
	
	::close(tcp_fd);
	tcp_fd = -1;
	
	debug_say("  socket(AF_INET,SOCK_STREAM,0)");
	int fd = socket(sa.sin_family,SOCK_STREAM,0);
	if (fd==-1) return true;
	
	// Get control flags
	debug_say("  fcntl(fd,F_GETFL,0)");
	int flags = fcntl(fd,F_GETFL,0);
	if (flags == -1) {
		debug_say("    Returned with errno "<<errno);
		::close(fd);
		return true;
	}
	
	// Set non-blocking mode
	debug_say("  fcntl(fd,F_SETFL,flags|O_NONBLOCK)");
	if (fcntl(fd,F_SETFL,flags|O_NONBLOCK)==-1) {
		debug_say("    Returned with errno "<<errno);
		::close(fd);
		return true;
	} 
	
	// Timeout parameters
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd,&fds);
	struct timeval tv;
	tv.tv_sec  =  msec/1000;
	tv.tv_usec = (msec%1000)*1000;
	
	// Request connection
	debug_say("  ::connect(fd, (struct sockaddr *)&sa, sizeof(sa))");
	if (::connect(fd, (struct sockaddr *)&sa, sal) == -1) {
		if (errno!=EINPROGRESS && errno!=EAGAIN && errno!=EWOULDBLOCK) {
			debug_say("    Returned with errno "<<errno);
			::close(fd);
			return true;
		}
	}
	debug_say("  select(fd+1, 0, &fds, 0, &tv)");
	if (select(fd+1, 0, &fds, 0, &tv) != 1) {
		// either select failed or connection timed out
		debug_say("    Returned with errno "<<errno);
		::close(fd);
		return true;
	} 
	
	// Return to blocking mode
	debug_say("  fcntl(fd,F_SETFL,flags&~O_NONBLOCK)");
	if (fcntl(fd,F_SETFL,flags&~O_NONBLOCK)==-1) {
		debug_say("    Returned with errno "<<errno);
		::close(fd);
		return true;
	}
	
	// Disable Nagle's algorithm (for reduced latency)
	int nd = 1;
	debug_say("  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nd, sizeof(nd))");
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nd, sizeof(nd))==-1) {
		debug_say("    Returned with errno "<<errno);
		::close(fd);
		return true;
	}
	
	// Everything went fine.
	debug_say("  Connection successfull.");
	tcp_fd = fd;
	return false;
}

void tcp_bus::close() {
	if (tcp_fd>=0) ::close(tcp_fd);
	tcp_fd = -1;
}

tcp_bus::operator bool () const {
	if (tcp_fd == -1) return false;
	int err = write(tcp_fd, 0, 0);
	if (err != -1) debug_say("Write on fd="<<tcp_fd<<" returned "<<err<<" errno="<<errno);
	return err == 0;
}

} // namespace modbus
