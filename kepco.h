#ifndef KEPCO_H
#define KEPCO_H

#include <string>
#include <cmath>
#include <cstdlib>
#include "straux.h"
#include "serial.h"

//#define DEBUG
#include "debug.h"

class kepco_bop {
	serialHandler serial;
	bool status; //true=ok, false=error
	std::string lasterror;
	bool echoed;
	
	static std::string float_to_string(double v);
	public:
	std::string exchange(std::string cmd);
	void clearError();
	std::string getLastError() {
		std::string s = lasterror;
		lasterror="";
		return s;
	}
	
	operator bool () const { return status; }
	
	bool reset() {
//		clearError();
		lasterror = exchange("*rst");
		return lasterror != "";
	}
	bool setVoltage(double v) {
		lasterror=exchange("volt " + float_to_string(v));
		return lasterror != "";
	}
	bool setCurrent(double i) {
		lasterror=exchange("curr " + float_to_string(i));
		return lasterror != "";
	}
	bool setVoltageAndCurrent(double v, double i) {
		lasterror=exchange("volt " + float_to_string(v) + ";curr " + float_to_string(i));
		return lasterror != "";
	}
	bool currentMode() {
		lasterror=exchange("func:mode curr");
		return lasterror != "";
	}
	bool voltageMode() {
		lasterror=exchange("func:mode volt");
		return lasterror != "";
	}
	double getVoltage() {
		std::string s = exchange("meas:volt?");
		if (!strIsFloat(s)) {
			lasterror = s;
			return NAN;
		}
		return std::atof(s.c_str());
	}
	double getCurrent() {
		std::string s = exchange("meas:curr?");
		if (!strIsFloat(s)) {
			lasterror = s;
			return NAN;
		}
		return std::atof(s.c_str());
	}
	bool getVoltageAndCurrent(double &v, double &i) {
		std::string s = exchange("meas:volt?;curr?");
		int n;
		for (n=0; s[n]!=';' && n<s.length(); ++n);
		if (n+1>=s.length()) {
			lasterror = "Expected \';\', not found.";
			return true; // Erro
		}
		v = std::atof(std::string(s,0,n).c_str());
		i = std::atof(std::string(s,n+1).c_str());
		return false;
	}
	string getName() {
		return exchange("*idn?");
	}
	
	bool open(std::string device, int baud=9600);
	void close() {
		if (status) reset();
		serial.Disconnect();
		status = false;
	}
	
	kepco_bop() : status(false), echoed(false) {}
	kepco_bop(std::string device, int baud=9600) : status(false), echoed(false) {
		open(device, baud);
	}
	~kepco_bop() { close(); }
};

#endif
