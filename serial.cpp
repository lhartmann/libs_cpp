/***************************************************************************
 *   Copyright (C) 2006 by Lucas V. Hartmann <lucas.hartmann@gmail.com>.   *
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

#include "serial.h"

//#define DUMP_BYTEINS
#ifdef DUMP_BYTEINS
#include <iostream>
#include <iomanip>
#include <cctype>
#endif
//---------------------------------------------------------------------------
serialHandler::serialHandler() {
	port=0;
	portHandle=INVALID_HANDLE_VALUE;
	portSpeed=B9600;
	portParity=prNone;
	portToRs=100;
	portToRm=10;
	portToRt=1000;
	portToWm=10;
	portToWt=1000;
}

bool serialHandler::Connect() {
	if (Connected()) return false;
	if (port>=0) {
		portName="/dev/ttyS";
		portName+=char('0'+port);
	}
	portHandle = ::open (portName.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
// 	portHandle = ::open ("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_SYNC);
	if (Connected()) {
		fcntl(portHandle,F_SETFL,0);
		setBaudRate(getBaudRate());
		setParity(portParity);
		setReadTimeouts(portToRs,portToRm,portToRt);
		setWriteTimeouts(portToWm,portToWt);
		struct termios options;
		tcgetattr(portHandle, &options);
		options.c_cflag &= ~(CSIZE|CSTOPB|CRTSCTS);
		options.c_cflag |= CS8|CREAD;
		options.c_iflag |= IGNBRK|IGNPAR;
		options.c_iflag &= ~(IXON|IXOFF|INPCK|IXANY|BRKINT|INLCR|ICRNL|IMAXBEL);
		options.c_oflag &= ~(OPOST);
		options.c_lflag |= NOFLSH;
		options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
		tcsetattr(portHandle, TCSANOW, &options);
		return false;
	}
	return true;
}

void serialHandler::Disconnect() {
	if (Connected()) {
		close(portHandle);
		portHandle=INVALID_HANDLE_VALUE;
	}
}

bool serialHandler::setPort(int p) {
	if (Connected()) return true;
	port=p;
	return false;
}

bool serialHandler::setPort(string s) {
	if (Connected()) return true;
	port=-1;
	portName=s;
	return false;
}

bool serialHandler::setBaudRate(int rate) {
	switch (rate) {
	case 110: portSpeed=B110; break;
	case 300: portSpeed=B300; break;
	case 600: portSpeed=B600; break;
	case 1200: portSpeed=B1200; break;
	case 2400: portSpeed=B2400; break;
	case 4800: portSpeed=B4800; break;
	case 9600: portSpeed=B9600; break;
//	case 14400: portSpeed=B14400; break;
	case 19200: portSpeed=B19200; break;
	case 38400: portSpeed=B38400; break;
	case 57600: portSpeed=B57600; break;
	case 115200: portSpeed=B115200; break;
	default: return true;
	}
	if (Connected()) {
		struct termios opt;
		tcgetattr(portHandle, &opt);
		cfsetispeed(&opt, portSpeed);
		cfsetospeed(&opt, portSpeed);
		opt.c_cflag |= CLOCAL | CREAD;
		tcsetattr(portHandle, TCSANOW, &opt);
	}
	return false;
}

int serialHandler::getBaudRate() {
	switch (portSpeed) {
	case B110: return 110;
	case B300: return 300;
	case B600: return 600;
	case B1200: return 1200;
	case B2400: return 2400;
	case B4800: return 4800;
	case B9600: return 9600;
//	case B14400: return 14400;
	case B19200: return 19200;
	case B38400: return 38400;
	case B57600: return 57600;
	case B115200: return 115200;
	}
	return 0;
}

// Fun��es de controle de RTS, CTS, DTR e DSR
bool serialHandler::cts() {
	if (!Connected()) return false;
	int i;
	ioctl(portHandle, TIOCMGET, &i);
	return i & TIOCM_CTS;
}

bool serialHandler::dsr() {
	if (!Connected()) return false;
	int i;
	ioctl(portHandle, TIOCMGET, &i);
	return i & TIOCM_DSR;
}

bool serialHandler::ri() {
	if (!Connected()) return false;
	int i;
	ioctl(portHandle, TIOCMGET, &i);
	return i & TIOCM_RI;
}

void serialHandler::rts(bool b) {
	if (!Connected()) return;
	int i;
	ioctl(portHandle, TIOCMGET, &i);
	if (b) i |= TIOCM_RTS;
	else i &= ~TIOCM_RTS;
	ioctl(portHandle, TIOCMGET, &i);
	lastRTS=b;
}

void serialHandler::dtr(bool b) {
	if (!Connected()) return;
	int i;
	ioctl(portHandle, TIOCMGET, &i);
	if (b) i |= TIOCM_DTR;
	else i &= ~TIOCM_DTR;
	ioctl(portHandle, TIOCMGET, &i);
	lastDTR=b;
}

// Fun��es de Timeout
bool serialHandler::setReadTimeouts(int single, int multiplier, int total) {
//	portTimeouts.ReadIntervalTimeout=single;
//	portTimeouts.ReadTotalTimeoutMultiplier=multiplier;
//	portTimeouts.ReadTotalTimeoutConstant=total;
//	return !SetCommTimeouts(portHandle,&portTimeouts);
	return false;
}

bool serialHandler::setWriteTimeouts(int multiplier, int total) {
//	portTimeouts.WriteTotalTimeoutMultiplier=multiplier;
//	portTimeouts.WriteTotalTimeoutConstant=total;
//	return !SetCommTimeouts(portHandle,&portTimeouts);
	return false;
}

// Funções para paridade
void serialHandler::setParity(enum parity p) {
	portParity=p;
	if (Connected()) {
		struct termios opt;
		tcgetattr(portHandle, &opt);
		switch (p) {
		case prNone:
			opt.c_cflag &= ~(PARENB|CSTOPB|CSIZE);
			opt.c_cflag|=CS8;
			break;
		case prOdd:
			opt.c_cflag &= ~(CSTOPB|CSIZE);
			opt.c_cflag|=CS8|PARENB|PARODD;
			break;
		case prEven:
			opt.c_cflag &= ~(CSTOPB|CSIZE|PARODD);
			opt.c_cflag|=CS8|PARENB;
			break;
		case prHigh:
			opt.c_cflag &= ~(PARENB|CSIZE);
			opt.c_cflag|=CS8|CSTOPB;
			break;
		case prLow: // N�o dispon�vel. Desativa paridade. (8n1)
			opt.c_cflag &= ~(PARENB|CSTOPB|CSIZE);
			opt.c_cflag|=CS8;
			break;
		}
		tcsetattr(portHandle, TCSANOW, &opt);
	}
}

// Funções de Entrada e Saída
inline int foolwrite(int &fd, const char *data, size_t nbytes) {
	return write(fd,data,nbytes);
}

bool serialHandler::write(const char *p, int n) {
	if (!Connected()) return true;
	return foolwrite(portHandle,p,n) < 0;
}

bool serialHandler::getch(char &ch) {
	if (!Connected()) return true;
	fd_set set;
	FD_ZERO((&set));
	FD_SET(portHandle,(&set));
	timeval timeout;
	timeout.tv_sec=0;
	timeout.tv_usec=500000;
	do {
		int i=select(portHandle+1,&set,0,0,&timeout);
		if (i<0) {
			if (errno = EINTR) continue;
			return true;
		} else if (i==0) return true;
	} while (false);
	int i = read(portHandle,&ch,1);
#ifdef DUMP_BYTEINS
	if (i) {
		std::cerr<<"Bytein: 0x"<<std::setw(2)<<std::setfill('0')<<std::hex<<unsigned(ch);
		if (std::isgraph(ch)) std::cerr<<", \'"<<ch<<"\'";
		std::cerr<<std::endl;
	}
#endif
	return !i;
}
//---------------------------------------------------------------------------
