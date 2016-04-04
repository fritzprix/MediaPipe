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
	ssize_t read(uint8_t* rb, size_t sz) const ;
	char read() const;
	ssize_t write(const uint8_t* wb, size_t sz) ;
	int write(const char c);
	int close() ;
private:
	int sock_fd;
	sockaddr_in dest_addr;
};

} /* namespace MediaPipe */

#endif /* CORE_IO_MEDIACLIENTSOCKETSTREAM_H_ */
