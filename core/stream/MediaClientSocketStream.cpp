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


namespace MediaPipe {

MediaClientSocketStream::MediaClientSocketStream(const char* host, int port) {

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0) {
		perror("fail to create socket!!\n");
		exit(EXIT_FAILURE);
	}

	struct hostent* resolved_host = gethostbyname(host);
	if(resolved_host == NULL){
		fprintf(stderr, "fail to resolve hostname : %s!!\n", host);
		exit(EXIT_FAILURE);
	}
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	::memcpy(&dest_addr.sin_addr,resolved_host->h_addr,sizeof(dest_addr.sin_addr));
}


MediaClientSocketStream::MediaClientSocketStream(int port)
{
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0) {
		perror("fail to create socket!!\n");
		exit(EXIT_FAILURE);
	}

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
	if(sock_fd < 0)	{
		perror("invalid state : socket is not initialized !!\n");
		exit(EXIT_FAILURE);
	}
	int res = ::connect(sock_fd, (const sockaddr*) &dest_addr, sizeof(sockaddr_in));
	if(res < 0) {
		perror("fail to connect server !!\n");
		exit(EXIT_FAILURE);
	}
	return res;
}

ssize_t MediaClientSocketStream::read(uint8_t* rb, size_t sz) const {
	if(sock_fd < 0) {
		fprintf(stderr, "socket not opened !! \n");
		exit(1);
	}
	return recv(sock_fd, rb, sz,0);
}

char MediaClientSocketStream::read() const {
	if(sock_fd < 0) {
		fprintf(stderr, "socket not opened !! \n");
		exit(1);
	}
	char c = 0;
	recv(sock_fd,&c,sizeof(char),0);
	return c;
}

ssize_t MediaClientSocketStream::write(const uint8_t* wb, size_t sz) {
	if(sock_fd < 0) {
		fprintf(stderr, "socket not opened !! \n");
		exit(1);
	}
	return send(sock_fd, wb, sz, 0);
}

int MediaClientSocketStream::write(const char c) {
	if(sock_fd < 0) {
		fprintf(stderr, "socket not opened !! \n");
		exit(1);
	}
	return send(sock_fd, &c, sizeof(char),0);
}

int MediaClientSocketStream::close() {
	if(sock_fd < 0)
		return EXIT_FAILURE;
	return ::close(sock_fd);
}

} /* namespace MediaPipe */
