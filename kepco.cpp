#include "kepco.h"
#include <cstdlib>

void kepco_bop::clearError() {
	char ch=0;
	serial.putch('\n');
	do {
		if (serial.getch(ch)) serial.putch('\n');
	} while (ch!='>');
}

std::string kepco_bop::exchange(std::string cmd) {
	if (!status) return "ERROR (Not connected)";
	
	// Send the command and check the echo
	int i=0;
	char ch;
	cmd += '\n';
	if (echoed) {
		serial.putch(cmd[i++]); // Sends the first byte before the loop
		do {
			serial.putch(cmd[i]); // Sends a new byte (while receiving the previous echo)
			if (serial.getch(ch)) return "TIMEOUT (ECHO)";
			if (ch!=cmd[i-1]) return "ERROR (ECHO)";
		} while (++i!=cmd.length()); // len is always 1+, as a '\n' is added.
		if (serial.getch(ch)) return "TIMEOUT (ECHO)"; // Gets the last byte
		if (ch!=cmd[i-1]) return "ERROR (ECHO)";
	} else serial.write(cmd);
	
	// Reads the response std::string
	std::string r;
	do {
		if (serial.getch(ch)) return "TIMEOUT (RESPONSE)";
		if (ch==0x11 || ch==0x13 || ch=='\r') continue; // XON and XOFF and LF
		r += ch;
	} while (ch!='\n');
	if (echoed) {
		do {
			if (serial.getch(ch)) return "TIMEOUT (RESPONSE)";
		} while (ch!='>');
	}
	if (r.length()==0) return "";
	return std::string(r,0,r.length()-1);
}

std::string kepco_bop::float_to_string(double v) {
	const char conv[] = "0123456789";
	std::string s;
	if (v<0) { s='-'; v*=-1; }
	int i = int(1000*v+.5);
	s += conv[i/10000%10];//=='0' ? ' ' : conv[i/10000%10];
	s += conv[i/1000 %10];
	s += '.';
	s += conv[i/100  %10];
	s += conv[i/10   %10];
	s += conv[i/1    %10];
	return s;
}

bool kepco_bop::open(std::string device, int baud) {
	serial.setPort(device);
	serial.setBaudRate(baud);
	serial.setParity(serialHandler::prNone);
	serial.setWriteTimeouts(2,200);
	serial.setReadTimeouts(100,2,200);
	serial.Connect();
	status=false;
	echoed=false;
	if (serial.Connected()) {
		// Check whether the power supply is echoed
		string s;
		while (!serial.getline(s)); // Read until timeout
		serial.putch('\n');
		while (!serial.getline(s)); // Read until timeout
		debug_dump_vector(s, s.length(), DEBUG_DUMP_CHAR_AS_HEX);
		echoed = s==">";
		
		status = true;
		s = exchange("*rst;*idn?");
		status = false;
		debug_dump(echoed);
		debug_dump(s);
		if      (s=="KEPCO BOP BIT232 REV. 3.1") status = true;
		else if (s=="KEPCO,BOP1KW 50-20 ,E156884, 2.01") status = true;
		else {
			serial.Disconnect();
		}
		lasterror = s;
	} else {
		lasterror="Serial connection failed.";
	}
	exchange("syst:rem on");
	exchange("outp on");
	return status;
}
