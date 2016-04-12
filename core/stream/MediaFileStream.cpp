/*
 * MediaFileStream.cpp
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#include "MediaFileStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

namespace MediaPipe {

MediaFileStream::MediaFileStream(const char* filename) {
	this->filename.append(filename);
	fd = -1;
}

MediaFileStream::~MediaFileStream() {
	this->close();
}

int MediaFileStream::open(void) {
	assert(!(fd >= 0));
	fd = ::open(filename.c_str(), O_RDWR);
	assert(!(fd < 0));
	return EXIT_SUCCESS;
}

ssize_t MediaFileStream::read(void* rb, size_t sz) const {
	assert(!(fd < 0));
	return ::read(fd, rb, sz);
}

uint8_t MediaFileStream::read() const {
	assert(!(fd < 0));
	uint8_t c = 0;
	::read(fd, &c, sizeof(uint8_t));
	return c;
}

ssize_t MediaFileStream::write(const void* wb, size_t sz) {
	assert(!(fd < 0));
	return ::write(fd, wb, sz);
}

ssize_t MediaFileStream::write(const uint8_t c) {
	assert(!(fd < 0));
	return ::write(fd, &c, sizeof(uint8_t));
}

int MediaFileStream::close() {
	if(fd < 0){
		return EXIT_FAILURE;
	}
	::close(fd);
	fd = -1;
	return EXIT_SUCCESS;
}


} /* namespace MediaPipe */
