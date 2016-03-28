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

namespace MediaPipe {

MediaFileStream::MediaFileStream(const char* filename) {
	this->filename.append(filename);
	fd = -1;
}

MediaFileStream::~MediaFileStream() {
	this->close();
}

int MediaFileStream::open(void) {
	if(fd >= 0) {
		::perror("file has already opened !!\n");
		return EXIT_FAILURE;
	}
	fd = ::open(filename.c_str(), O_RDWR);
	if(fd < 0) {
		::perror("fail to open file !!\n");
		::exit(-1);
	}
	return EXIT_SUCCESS;
}

ssize_t MediaFileStream::read(uint8_t* rb, size_t sz) const {
	if(fd < 0) {
		::perror("illegal state : file not opened !! \n");
		::exit(-1);
	}
	return ::read(fd, rb, sz);
}

ssize_t MediaFileStream::write(const uint8_t* wb, size_t sz) {
	if(fd < 0) {
		::perror("illegal state : file not opened !! \n");
		::exit(-1);
	}
	return ::write(fd, wb, sz);
}

int MediaFileStream::close() {
	if(fd > 0) {
		return ::close(fd);
		fd = -1;
	}
	return EXIT_SUCCESS;
}



} /* namespace MediaPipe */
