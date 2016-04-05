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

ssize_t MediaFileStream::read(uint8_t* rb, size_t sz) const {
	assert(!(fd < 0));
	return ::read(fd, rb, sz);
}

char MediaFileStream::read() const {
	assert(!(fd < 0));
	char c = 0;
	::read(fd, &c, sizeof(char));
	return c;
}

ssize_t MediaFileStream::write(const uint8_t* wb, size_t sz) {
	assert(!(fd < 0));
	return ::write(fd, wb, sz);
}

int MediaFileStream::write(const char c) {
	assert(!(fd < 0));
	return ::write(fd, &c, sizeof(char));
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
