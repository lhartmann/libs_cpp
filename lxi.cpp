#include "lxi.h"
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

//#define DEBUG
#include "debug.h"

using namespace std;

#define LXI_DEFAULT_PORT htons(5025)
#define LXI_DEFAULT_CONECT_TIMEOUT 120000
#define LXI_DEFAULT_MESSAGE_TIMEOUT 120000

int lxiConnectWithTimeout(uint32_t msec, uint32_t address, uint16_t port) {
	debug_say("lxiConnectWithTimeout("<<msec<<", "<<address<<", "<<port<<")");
	
	// Create a socket
	int fd = socket(PF_INET,SOCK_STREAM,0);
	if (fd==-1) return -1;

	// Get control flags
	int flags = fcntl(fd,F_GETFL,0);
	if (flags == -1) {
        ::close(fd);
		return -1;
	}
	
	// Set non-blocking mode
	if (fcntl(fd,F_SETFL,flags|O_NONBLOCK)==-1) {
        ::close(fd);
		return -1;
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
	connect(fd,(struct sockaddr *)&sa, sizeof(sa));
	if (select(fd+1, 0, &fds, 0, &tv) != 1) {
		// either select failed or connection timed out
        ::close(fd);
		return -1;
	}
	
	// Return to blocking mode
	if (fcntl(fd,F_SETFL,flags&~O_NONBLOCK)==-1) {
        ::close(fd);
		return -1;
	}
	
	// Disable Nagle's algorithm (for reduced latency)
	int nd = 1;
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nd, sizeof(nd))==-1) {
        ::close(fd);
		return -1;
	}
	
	// Everything went fine.
	return fd;
}

// lxiMessage
bool lxiMessageWithTimeout(int fd, std::string cmd, std::string *answer, uint32_t msec) {
	debug_say("lxiMessageWithTimeout("<<fd<<", \""<<cmd<<"\", "<<answer<<", "<<msec<<")");
	
	cmd+='\n';
	int i = write(fd, cmd.c_str(), cmd.length());
	if (i<0) {
		debug_say("  Send request failed, errno="<<errno<<".");
		return true;
	}
	
	if (!answer) return false; // No answer expected
	
	// Timeout parameters
	fd_set rfds, efds;
	FD_ZERO(&rfds);
	FD_SET(fd,&rfds);
	FD_ZERO(&efds);
	FD_SET(fd,&efds);
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
	int rcvd=0;
	while (1) {
		if (select(fd+1, &rfds, 0, &efds, &tv) != 1) {
			// either select failed or connection timed out
			debug_say("  Timeout ou select failed ("<<errno<<").");
			return true;
		}
		
		const long len=10240;
		char buffer[10240];
		i = read(fd, buffer+rcvd, len-rcvd);
		if (i<1) return true;
		rcvd += i;
		if (buffer[rcvd-1] == '\n') {
			--rcvd; // remove trailing newline
			*answer = std::string(buffer,rcvd);
			break;
		}
		gettimeofday(&now,0);
		if (now.tv_sec > end.tv_sec) {
			debug_say("  Timeout.");
			return true;
		}
		if (now.tv_sec < end.tv_sec) continue;
		if (now.tv_usec > end.tv_usec) {
			debug_say("  Timeout.");
			return true;
		}
	}
	debug_say("  answer=\""<<*answer<<"\"")
	return false;
}

uint32_t lxiScanRange(std::string idn, uint32_t ip0, uint32_t ip1, uint16_t port) {
	ip0=ntohl(ip0);
	ip1=ntohl(ip1);
	if (ip0>ip1) return 0;
	
	for (uint32_t ip=ip0; ip<=ip1; ++ip) {
		int fd = lxiConnectWithTimeout(30, htonl(ip));
		if (fd==-1) continue;
		std::string s = lxiIdentify(fd);
		if (idn == s) return htonl(ip);
	}
	
	return 0;
}
