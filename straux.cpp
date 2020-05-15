/***************************************************************************
 *   Copyright (C) 2016 Lucas V. Hartmann <lucas.hartmann@gmail.com>       *
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

#include "straux.h"
#include <cctype>
#include <cstdio>
#include <cmath>

using namespace std;

static const char *const conv = "0123456789ABCDEF";

bool strIsFloat(std::string str) {
	int state=0;
	for (int i=0; i<str.length(); ++i) {
		char ch = str[i];
		switch (state) {
		case 0: // Blanks at start
			if (isblank(ch))        { state=0; break; }
			if (ch=='+' || ch=='-') { state=1; break; }
			if (isdigit(ch))        { state=2; break; }
			if (ch=='.')            { state=7; break; }
			return false;
		case 1: // After initial sign
			if (isdigit(ch))        { state=2; break; }
			if (ch=='.')            { state=7; break; }
			return false;
		case 2: // Received unit mantissa digits
			if (isdigit(ch))        { state=2; break; }
			if (ch=='.')            { state=3; break; }
			if (ch=='e' || ch=='E') { state=4; break; }
			return false;
		case 3: // Received decimal point
			if (isdigit(ch))        { state=3; break; }
			if (ch=='e' || ch=='E') { state=4; break; }
			return false;
		case 4: // Received 'e'
			if (ch=='+' || ch=='-') { state=5; break; }
			if (isdigit(ch))        { state=6; break; }
			return false;
		case 5: // Received 'e'
			if (isdigit(ch))        { state=6; break; }
			return false;
		case 6: // Received exponent digits
			if (isdigit(ch))        { state=6; break; }
			return false;
		case 7: // Received decimal point before mantissa digits
			if (isdigit(ch))        { state=3; break; }
			if (ch=='e' || ch=='E') { state=4; break; }
			return false;
		}
	}
	// Marked states (where EOS means a valid double)
	return state==2 || state==3 || state==6;
}

static int valueof(char ch) {
	if (ch>='0' && ch<='9') return ch-'0';
	if (ch>='a' && ch<='f') return ch-'a'+10;
	if (ch>='A' && ch<='F') return ch-'A'+10;
	return 0;
}

/*double strToFloat(std::string str) {
	int state=0;
	double mi  = 0;
	double mf  = 0;
	double mfd = 1;
	double ex  = 0;
	for (int i=0; i<str.length(); ++i) {
		char ch = str[i];
		switch (state) {
		case 0: // Blanks at start
			if (isblank(ch))        { state=0; break; }
			if (ch=='+' || ch=='-') { state=1; break; }
			if (isdigit(ch))        { state=2; mi=mi*10+valueof(ch); break; }
			if (ch=='.')            { state=7; break; }
			return NAN;
		case 1: // After initial sign
			if (isdigit(ch))        { state=2; mi=mi*10+valueof(ch); break; }
			if (ch=='.')            { state=7; break; }
			return NAN;
		case 2: // Received unit mantissa digits
			if (isdigit(ch))        { state=2; mi=mi*10+valueof(ch); break; }
			if (ch=='.')            { state=3; break; }
			if (ch=='e' || ch=='E') { state=4; break; }
			return NAN;
		case 3: // Received decimal point
			if (isdigit(ch))        { state=3; mf=mf*10+valueof(ch); mfd*=10; break; }
			if (ch=='e' || ch=='E') { state=4; break; }
			return NAN;
		case 4: // Received 'e'
			if (ch=='+' || ch=='-') { state=5; break; }
			if (isdigit(ch))        { state=6; ex=ex*10+valueof(ch); break; }
			return NAN;
		case 5: // Received 'e'
			if (isdigit(ch))        { state=6; ex=ex*10+valueof(ch); break; }
			return NAN;
		case 6: // Received exponent digits
			if (isdigit(ch))        { state=6; ex=ex*10+valueof(ch); break; }
			return NAN;
		case 7: // Received decimal point before mantissa digits
			if (isdigit(ch))        { state=3; mf=mf*10+valueof(ch); mfd*=10; break; }
			if (ch=='e' || ch=='E') { state=4; break; }
			return NAN;
		}
	}
	// Marked states (where EOS means a valid double)
	if (state==2 || state==3 || state==6) {
		return (mi + mf/mfd) * pow(10, ex);
	}
	return NAN;
}*/

std::string floatToString(double d, int precision) {
	char fmt[512], ret[512];
	
	sprintf(fmt, "%%+.%dle", precision);
	sprintf(ret, fmt, d);
	return string(ret);
/*
	string r;
	if (d<0) {
		r+='-';
		d=-d;
	}
	int p10 = int(std::log10(d));
	d /= std::pow(10.,p10); // From now on 0 <= d < 10 is guaranteed.
	r += conv[int(d)];
	r += '.';
	while (--precision) {
		d = (d - int(d)) * 10;
		r += conv[int(d)];
	}
	r+='e';
	if (p10<0) {
		r += '-';
		p10 =- p10;
	}
	return r+intToString(p10);*/
}

bool strIsInt(std::string str) {
	int state=0;
	for (int i=0; i<str.length(); ++i) {
		char ch = str[i];
		switch (state) {
		case 0: // Blanks at start
			if (isblank(ch))        { state=0; break; }
			if (ch=='+' || ch=='-') { state=1; break; }
			if (isdigit(ch))        { state=2; break; }
			return false;
		case 1: // After initial sign
			if (isdigit(ch))        { state=2; break; }
			return false;
		case 2: // Received unit mantissa digits
			if (isdigit(ch))        { state=2; break; }
			return false;
		}
	}
	// Marked states (where EOS means a valid integger)
	return state==2;
}

std::vector<std::string> strSplit(std::string str, char div) {
	std::vector<std::string> ret;
	int j=0;
	for (int i=0; i<str.length(); ++i) {
		if (str[i]==div) {
			ret.push_back(std::string(str,j,i-j));
			j=i+1;
		}
	}
	ret.push_back(std::string(str,j));
	return ret;
}

std::string intToString(long int i, int digits, int base) {
	std::string ret;
	bool neg = i<0;
	if (neg) i=-i;
	if (base <  2) base = 2;
	if (base > 16) base = 16;
	do {
		ret = conv[i%base] + ret;
		i /= base;
		--digits;
	} while (i);
	while (digits-- > 0) ret = '0' + ret;
	if (neg) ret = '-' + ret;
	return ret;
}

static unsigned char deconv[256];
static bool built_deconv = false;
static void build_deconv() {
	for (int i=0; i<256; ++i) deconv[i] = 255;
	deconv['0'] = 0;
	deconv['1'] = 1;
	deconv['2'] = 2;
	deconv['3'] = 3;
	deconv['4'] = 4;
	deconv['5'] = 5;
	deconv['6'] = 6;
	deconv['7'] = 7;
	deconv['8'] = 8;
	deconv['9'] = 9;
	deconv['A'] = 10;
	deconv['B'] = 11;
	deconv['C'] = 12;
	deconv['D'] = 13;
	deconv['E'] = 14;
	deconv['F'] = 15;
	deconv['a'] = 10;
	deconv['b'] = 11;
	deconv['c'] = 12;
	deconv['d'] = 13;
	deconv['e'] = 14;
	deconv['f'] = 15;
	built_deconv = true;
}

unsigned long strToInt(std::string s, int base) {
	if (!built_deconv) build_deconv();
	
	unsigned long r = 0;
	for (int i=0; i<s.length(); ++i) {
		if (deconv[s[i]] >= base) return r;
		r = r*base + deconv[s[i]];
	}
	return r;
}

double strToTimestamp(string s) {
	double acc=0, x=0, k=1;
	for (int i=0; i<s.length(); ++i) {
		char ch=s[i];
		if (ch==':') {
			acc = (acc+x)*60;
			x=0;
		} else if (ch==',') {
			acc = acc+x;
			x   = 0;
			k   = 0.1;
		} else if (ch<'0' || ch>'9') {
			return -1; // error
		} else if (k>=1) {
			x = x*10 + ch-'0';
		} else {
			x += k*(ch-'0');
			k /= 10;
		}
	}
	return acc+x;
}

string timestampToString(double d) {
	string ret;
	if (d<0) {
		ret='-';
		d = -d;
	}
	ret += intToString(int(d/3600)) + ':';
	d -= 3600*floor(d/3600);
	ret += intToString(int(d/60),2) + ':';
	d -= 60*floor(d/60);
	ret += intToString(int(d),2) + ',';
	d -= floor(d);
	ret += intToString(int(d*1000),3);

	return ret;
}

bool strIsTimestamp(string s) {
	if (s.length()!=12) return false;
	if (!isdigit(s[0])) return false;
	if (!isdigit(s[1])) return false;
	if (s[2]!=':') return false;
	if (!isdigit(s[3])) return false;
	if (!isdigit(s[4])) return false;
	if (s[5]!=':') return false;
	if (!isdigit(s[6])) return false;
	if (!isdigit(s[7])) return false;
	if (s[8]!=',') return false;
	if (!isdigit(s[9])) return false;
	if (!isdigit(s[10])) return false;
	if (!isdigit(s[11])) return false;
	return true;
}

string strClean(string s) {
	string r, x;
	string blank=" \t\f\b\r\n";
	for (int i=0; i<s.length(); ++i) {
		if (blank.find_first_of(s[i]) == string::npos) {
			if (x.length() && r.length()) r+=x;
			x="";
			r+=s[i];
		} else {
			x+=s[i];
		}
	}
	return r;
}

struct strEscape_pair {
	char c;
	char ec;
};

static const strEscape_pair strEscape_chars[] = {
	// Standard C escape chars
	{ '\r', 'r' },
	{ '\n', 'n' },
	{ '\a', 'a' },
	{ '\b', 'b' },
	{ '\f', 'f' },
	{ '\\', '\\' },
	{ '\'', '\'' },
	{ '\"', '\"' },
	{ '\t', 't' },
	{ '\v', 'v' },
	{ 0, '\n' },
	// Additional chars to escape (sanitize *nix command line arguments)
	{ ' ', ' ' },
	{ '`', '`' },
	{ '?', '?' },
	{ '$', '$' },
	{ ';', ';' },
//	{ ':', ':' },
	{ '[', '[' }, { ']', ']' },
	{ '{', '{' }, { '}', '}' },
	{ '(', '(' }, { ')', ')' },
	{ 0, 0 }
};

string strEscape(string s, bool unix_style) {
	string r;
	int nchars = unix_style ? 23 : 11;
	for (int i=0; i<s.length() && s[i]; ++i) {
		int j=0;
		while (j<nchars && strEscape_chars[j].c!=s[i]) j++;
		if (j == nchars) r += s[i];
		else { r+='\\'; r+=strEscape_chars[j].ec; }
	}
	return r;
}

string strUnescape(string s, bool unix_style) {
	string r;
	int nchars = unix_style ? 23 : 11;
	for (int i=0; i<s.length() && s[i]; ++i) {
		if (s[i] == '\\') {
			++i;
			if (i<s.length() && s[i]) {
				int j=0;
				while (j<nchars && strEscape_chars[j].ec!=s[i]) j++;
				if (j == nchars) r += s[i];
				else r+=strEscape_chars[j].c;
			} else break;
		}
	}
	return r;
}
