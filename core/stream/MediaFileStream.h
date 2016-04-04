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
	ssize_t read(uint8_t* rb, size_t sz) const;
	char read() const;
	ssize_t write(const uint8_t* wb, size_t sz);
	int write(const char);
	int close();

private:
	std::string filename;
	int fd;
};

} /* namespace MediaPipe */

#endif /* CORE_IO_MEDIAFILESTREAM_H_ */
