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

#include "modbus_serial.h"
#include "crc16.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

namespace modbus {

void serial_bus::open(const char *devname, unsigned long baud) {
	if (serial_fd>0) ::close(serial_fd);
	serial_fd = -1;
	
	// Change value into Bxxx flag
	tcflag_t B = 
		baud==     50 ? B50     :
		baud==     75 ? B75     :
		baud==    110 ? B110    :
		baud==    134 ? B134    :
		baud==    150 ? B150    :
		baud==    200 ? B200    :
		baud==    300 ? B300    :
		baud==    600 ? B600    :
		baud==   1200 ? B1200   :
		baud==   1800 ? B1800   :
		baud==   2400 ? B2400   :
		baud==   4800 ? B4800   :
		baud==   9600 ? B9600   :
		baud==  19200 ? B19200  :
		baud==  38400 ? B38400  :
		baud==  57600 ? B57600  :
		baud== 115200 ? B115200 :
		0;
	if (!B) return;
	
	// Open Device
	serial_fd = ::open(devname, O_RDWR | O_NOCTTY | O_NDELAY);
	if (serial_fd<0) return;
	
	// Set Options
	struct termios options;
	if (tcgetattr(serial_fd, &options)) {
		::close(serial_fd);
		serial_fd = -1;
		return;
	}
	options.c_cflag &= ~(CSIZE | CSTOPB | PARENB | CRTSCTS);
	options.c_cflag |= CS8 | CREAD | HUPCL;
	cfsetispeed(&options, B);
	cfsetospeed(&options, B);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_iflag &= ~(IXON | IXOFF | IXANY | INLCR | ICRNL | IUCLC | IMAXBEL | BRKINT);
	options.c_iflag |= IGNPAR;
	options.c_oflag &= ~(OPOST);
	if (tcsetattr(serial_fd, TCSANOW, &options)) {
		::close(serial_fd);
		serial_fd = -1;
		return;
	}
}

void serial_bus::close() {
	if (serial_fd>=0) ::close(serial_fd);
	serial_fd = -1;
}

serial_bus::operator bool () const {
	return serial_fd >= 0;
}

} // namespace modbus
