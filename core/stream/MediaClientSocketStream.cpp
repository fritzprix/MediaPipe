/*
 * MediaClientSocketStream.cpp
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#include "MediaClientSocketStream.h"

#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

static size_t READOUT_BUFFER_SIZE = 16;

namespace MediaPipe {

MediaClientSocketStream::MediaClientSocketStream(const char* host, int port) {

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	assert(!(sock_fd < 0));

	struct hostent* resolved_host = gethostbyname(host);
	assert(resolved_host);

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	::memcpy(&dest_addr.sin_addr,resolved_host->h_addr,sizeof(dest_addr.sin_addr));
}


MediaClientSocketStream::MediaClientSocketStream(int port)
{
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	assert(!(sock_fd < 0));

	// setup loopback address
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

MediaClientSocketStream::~MediaClientSocketStream() {
	this->close();
}


int MediaClientSocketStream::open(void) {
	assert(!(sock_fd < 0));
	int res = connect(sock_fd, (const sockaddr*) &dest_addr, sizeof(sockaddr_in));
	assert(!(res < 0));
	return res;
}

ssize_t MediaClientSocketStream::read(void* rb, size_t sz) const {
	assert(!(sock_fd < 0));
	return recv(sock_fd, rb, sz,0);
}

uint8_t MediaClientSocketStream::read() const {
	assert(!(sock_fd < 0));
	uint8_t c = 0;
	recv(sock_fd,&c,sizeof(uint8_t),0);
	return c;
}

ssize_t MediaClientSocketStream::write(const void* wb, size_t sz) {
	assert(!(sock_fd < 0));
	return send(sock_fd, wb, sz, 0);
}

ssize_t MediaClientSocketStream::write(const uint8_t c) {
	assert(!(sock_fd < 0));
	return send(sock_fd, &c, sizeof(uint8_t),0);
}

ssize_t MediaClientSocketStream::skip(size_t sz) const
{
	assert((sz > 0) && !(sock_fd < 0));
	uint8_t buffer[READOUT_BUFFER_SIZE];
	size_t res, rsz = sz;
	while(rsz > READOUT_BUFFER_SIZE)
	{
		if((res = recv(sock_fd, buffer, READOUT_BUFFER_SIZE, 0)) < 0)
			return res;
		rsz -= res;
	}
	if((res = recv(sock_fd, buffer,rsz, 0)) < 0)
		return res;
	rsz -= res;
	return (sz - rsz);
}


int MediaClientSocketStream::close() {
	if(sock_fd < 0)
		return EXIT_FAILURE;
	::close(sock_fd);
	sock_fd = -1;
	return EXIT_SUCCESS;
}

} /* namespace MediaPipe */
