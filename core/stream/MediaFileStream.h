/*
 * MediaFileStream.h
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#ifndef CORE_IO_MEDIAFILESTREAM_H_
#define CORE_IO_MEDIAFILESTREAM_H_

#include "MediaStream.h"
#include <string>

namespace MediaPipe {

class MediaFileStream : public MediaStream {
public:
	MediaFileStream(const char* filename);
	virtual ~MediaFileStream();

	int open(void);
	ssize_t read(void* rb, size_t sz) const;
	uint8_t read() const;
	ssize_t write(const void* wb, size_t sz);
	ssize_t write(const uint8_t);
	int close();

private:
	std::string filename;
	int fd;
};

} /* namespace MediaPipe */

#endif /* CORE_IO_MEDIAFILESTREAM_H_ */
