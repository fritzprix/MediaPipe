/*
 * MediaServerSocketStream.cpp
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#include "MediaServerSocketStream.h"

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <cdsl_nrbtree.h>
#include <unistd.h>
#include <assert.h>



struct client_session {
	nrbtreeNode_t node;
	int client_fd;
	sockaddr_in addr;
};

namespace MediaPipe {

MediaServerSocketStream::MediaServerSocketStream(const char* host, int port) {
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	assert(!(sock_fd < 0));
	client_fd = -1;
	struct hostent* resolved_host = gethostbyname(host);
	assert(resolved_host);

	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(port);
	memcpy(&host_addr.sin_addr, resolved_host->h_addr, sizeof(host_addr.sin_addr));
	cdsl_nrbtreeRootInit(this);
}

MediaServerSocketStream::MediaServerSocketStream(int port) {
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	assert(!(sock_fd < 0));
	client_fd = -1;

	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(port);
	host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cdsl_nrbtreeRootInit(this);
}


MediaServerSocketStream::~MediaServerSocketStream() {
	this->close();
}


int MediaServerSocketStream::open(void) {
	socklen_t sl = sizeof(sockaddr_in);
	sockaddr_in client_addr;
	int res;
	res = bind(sock_fd, (sockaddr*) &host_addr, sl);
	assert(!(res < 0));
	res = listen(sock_fd,0);
	assert(!(res < 0));
	client_fd = accept(sock_fd, (sockaddr*) &client_addr,&sl);
	assert(!(client_fd < 0));
	return EXIT_SUCCESS;
}

ssize_t MediaServerSocketStream::read(void* rb, size_t sz) const {
	assert(!(client_fd < 0));
	return recv(client_fd, rb, sz,0);
}

uint8_t MediaServerSocketStream::read() const {
	assert(!(client_fd < 0));
	uint8_t c = 0;
	recv(sock_fd,&c,sizeof(uint8_t),0);
	return c;
}

ssize_t MediaServerSocketStream::write(const void* wb, size_t sz) {
	assert(!(client_fd < 0));
	return send(client_fd, wb, sz,0);
}

ssize_t MediaServerSocketStream::write(const uint8_t c) {
	assert(!(client_fd));
	return send(sock_fd, &c, sizeof(uint8_t),0);
}

int MediaServerSocketStream::close() {
	if(sock_fd < -1){
		return EXIT_FAILURE;
	}

	::close(sock_fd);
	sock_fd = -1;
	client_fd = -1;
	return EXIT_SUCCESS;
}

} /* namespace MediaPipe */
