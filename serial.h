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

#ifndef SERIAL_H
#define SERIAL_H

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <string>

using std::string;
#define INVALID_HANDLE_VALUE -1

class serialHandler {
	public:
	enum parity {
		prNone=0,
		prOdd,
		prEven,
		prHigh,
		prLow,
		prMark=prHigh,
		prSpace=prLow
	};
	private:
	int port;
	string portName;
	int portHandle;
	int portSpeed;
	enum parity portParity;
	int portToRs;
	int portToRm;
	int portToRt;
	int portToWm;
	int portToWt;
	bool lastRTS, lastDTR;
	public:
	serialHandler();
	bool Connected() const { return portHandle!=INVALID_HANDLE_VALUE; }
	operator bool() const { return portHandle!=INVALID_HANDLE_VALUE; }
	bool Connect();
	void Disconnect();
	~serialHandler() { Disconnect(); }
	bool setPort(int p);
	bool setPort(string s);
	int getPort() const { return port; }
	bool setBaudRate(int rate);
	int getBaudRate();
	// Funções de controle para RTS, CTS, DSR e DTR
	bool rts() { return lastRTS; }
	bool dtr() { return lastDTR; }
	bool cts();
	bool dsr();
	bool ri();
	void rts(bool b);
	void dtr(bool b); 
	// Funções de Timeout
	bool setReadTimeouts(int single, int multiplier, int total);
	bool setWriteTimeouts(int multiplier, int total);
	// Funções para paridade
	void setParity(enum parity p);
	enum parity getParity() { return portParity; }
	// Funções de Entrada e Saída
	bool write(string s) { return write(s.c_str(),s.length()); }
	bool write(const char *p, int n);
	bool getch(char &ch);
	bool getch(unsigned char &ch) {
		char c;
		bool b = getch(c);
		ch = c;
		return b;
	}
	bool getline(string &s) {
		s = "";
		char ch;
		while (!getch(ch)) {
			if (ch=='\n') return false;
			if (ch=='\r') continue;
			s += ch;
		}
		return true;
	}
	bool putch(char ch) { return write(&ch,1); }
	bool putch(unsigned char ch) { return write((char*) &ch,1); }
	void sendBreak() {
		if (!Connected()) return;
		tcsendbreak(portHandle,0);
	}
	void flush() {
		if (!Connected()) return;
		tcflush(portHandle,TCIOFLUSH);
	}
	void flushRead() {
		if (!Connected()) return;
		tcflush(portHandle,TCIFLUSH);
	}
	void flushWrite() {
		if (!Connected()) return;
		tcflush(portHandle,TCOFLUSH);
	}
};

#endif
