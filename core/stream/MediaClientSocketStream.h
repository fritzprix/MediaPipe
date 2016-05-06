/*
 * MediaClientSocketStream.h
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#ifndef CORE_IO_MEDIACLIENTSOCKETSTREAM_H_
#define CORE_IO_MEDIACLIENTSOCKETSTREAM_H_

#include "core/MediaStream.h"

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace MediaPipe {

class MediaClientSocketStream : public MediaStream {
public:
	MediaClientSocketStream(const char* host, int port);
	MediaClientSocketStream(int port);
	virtual ~MediaClientSocketStream();

	int open(void) ;
	ssize_t read(void* rb, size_t sz) const ;
	uint8_t read() const;
	ssize_t write(const void* wb, size_t sz);
	ssize_t write(const uint8_t c);
	ssize_t skip(size_t sz) const;
	int close() ;
private:
	int sock_fd;
	sockaddr_in dest_addr;
};

} /* namespace MediaPipe */

#endif /* CORE_IO_MEDIACLIENTSOCKETSTREAM_H_ */
