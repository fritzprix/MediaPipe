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



struct client_session {
	nrbtreeNode_t node;
	int client_fd;
	sockaddr_in addr;
};

namespace MediaPipe {

MediaServerSocketStream::MediaServerSocketStream(const char* host, int port) {
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	client_fd = -1;
	if(sock_fd < 0) {
		perror("fail to create socket !!\n");
		exit(EXIT_FAILURE);
	}
	struct hostent* resolved_host = gethostbyname(host);
	if(resolved_host == NULL) {
		fprintf(stderr, "fail to resolve hostname : %s!!\n", host);
		exit(EXIT_FAILURE);
	}

	memset(&host_addr, 0, sizeof(host_addr));
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(port);
	memcpy(&host_addr.sin_addr, resolved_host->h_addr, sizeof(host_addr.sin_addr));
	cdsl_nrbtreeRootInit(this);
}

MediaServerSocketStream::MediaServerSocketStream(int port) {
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	client_fd = -1;
	if(sock_fd < 0) {
		perror("fail to create socket !!\n");
		exit(EXIT_FAILURE);
	}

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
	if(	bind(sock_fd, (sockaddr*) &host_addr, sl) < 0 )
	{
		fprintf(stderr, "fail to bind server socket !! /w errcode : %d\n", errno);
		return errno;
	}
	if(listen(sock_fd, 0) < 0)
	{
		fprintf(stderr, "unexpected error in listen !! /w errcode : %d\n", errno);
		return errno;
	}
	if((client_fd = accept(sock_fd,(sockaddr*) &client_addr,&sl)) < 0)
	{
		fprintf(stderr, "fail to connect client !! /w errcode :%d\n", errno);
		return errno;
	}
	return EXIT_SUCCESS;
}

ssize_t MediaServerSocketStream::read(uint8_t* rb, size_t sz) const {
	if(client_fd < 0)
		return -1;
	return recv(client_fd, rb, sz,0);
}

ssize_t MediaServerSocketStream::write(const uint8_t* wb, size_t sz) {
	if(client_fd < 0)
		return -1;
	return send(client_fd, wb, sz,0);
}

int MediaServerSocketStream::close() {
	if(sock_fd < -1)
		return EXIT_FAILURE;
	return ::close(sock_fd);
}

} /* namespace MediaPipe */
