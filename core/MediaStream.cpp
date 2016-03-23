/*
 * MediaStream.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: innocentevil
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <error.h>
#include <errno.h>

#include "MediaStream.h"

namespace MediaPipe {

MediaStream::MediaStream(void){ }
MediaStream::~MediaStream(void){ }


MediaFileStream::MediaFileStream(const char* fname)
{
	filename = new std::string(fname);
	fd = -1;
}

MediaFileStream::~MediaFileStream()
{
	delete filename;
	close();
}

int MediaFileStream::open()
{
	fd = ::open(filename->c_str(), O_RDWR);
	if(fd < 0)
	{
		perror("File not available !! ");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

int MediaFileStream::close()
{
	if(fd < 0)
		return EXIT_FAILURE;
	return ::close(fd);
}

ssize_t MediaFileStream::read(uint8_t* rb, size_t sz) const
{
	if(fd < 0)
	{
		perror("File must be opened before read !!");
		exit(fd);
	}
	return ::read(fd, rb, sz);
}

ssize_t MediaFileStream::write(const uint8_t* wb, size_t sz)
{
	if(fd < 0)
	{
		perror("File must be opened before write !!");
		exit(fd);
	}
	return ::write(fd, wb, sz);
}

}

