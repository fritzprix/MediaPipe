/*
 * MediaServerSocketStream.h
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#ifndef CORE_IO_MEDIASERVERSOCKETSTREAM_H_
#define CORE_IO_MEDIASERVERSOCKETSTREAM_H_

#include "core/MediaStream.h"
#include "cdsl_nrbtree.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <arpa/inet.h>

namespace MediaPipe {

class MediaServerSocketStream : public MediaStream, nrbtreeRoot_t {
public:
	MediaServerSocketStream(const char* host, int port);
	MediaServerSocketStream(int port);
	virtual ~MediaServerSocketStream();

	int open(void);
	ssize_t read(uint8_t* rb, size_t sz) const;
	ssize_t write(const uint8_t* wb, size_t sz);
	int close();
private:
	int sock_fd;
	int client_fd;
	sockaddr_in host_addr;
};

} /* namespace MediaPipe */

#endif /* CORE_IO_MEDIASERVERSOCKETSTREAM_H_ */
