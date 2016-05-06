/*
 * MediaStream.h
 *
 *
 *  MediaStream is generic byte steam which pipe from one MediaPipe component to another
 *  MediaPipe component has to inherit Serializable to be sent to / received from MediaStream
 *
 *  Created on: Mar 16, 2016
 *      Author: innocentevil
 */

#ifndef MEDIASTREAM_H_
#define MEDIASTREAM_H_

#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace MediaPipe {

class MediaStream {
public:

	MediaStream(){};
	virtual ~MediaStream(){};

	virtual int open(void) = 0;
	virtual ssize_t read(void* rb, size_t sz) const = 0;
	virtual uint8_t read() const = 0;
	virtual ssize_t write(const void* wb, size_t sz) = 0;
	virtual ssize_t write(const uint8_t ) = 0;
	virtual ssize_t skip(size_t sz) const = 0;
	virtual int close() = 0;
};


}

#endif /* MEDIASTREAM_H_ */
