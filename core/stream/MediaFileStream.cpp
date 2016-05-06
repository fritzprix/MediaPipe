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

static size_t READOUT_BUFFER_SIZE = 16;

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
	fd = ::open(filename.c_str(), O_RDWR | O_CREAT, S_IRWXU);
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
	ssize_t res;
	if((res = ::read(fd, &c, sizeof(uint8_t))) < 0)
	{
		perror("Read Error !!");
		exit(res);
	}
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

ssize_t MediaFileStream::skip(size_t sz) const
{
	assert((sz > 0) && !(fd < 0));
	uint8_t buffer[READOUT_BUFFER_SIZE];
	size_t res, rsz = sz;
	while(rsz > READOUT_BUFFER_SIZE)
	{
		if((res = ::read(fd, buffer,READOUT_BUFFER_SIZE)) <0)
			return res;
		rsz -= res;
	}
	if((res = ::read(fd, buffer, rsz)) < 0)
		return res;
	rsz -= res;
	return (sz - rsz);
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
