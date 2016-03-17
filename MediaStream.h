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

class MediaContext {
public:
	virtual ~MediaContext();
};

class MediaStream {
public:

	class Serializable {
	public:
		Serializable();
		virtual ~Serializable();
		virtual ssize_t serialize(MediaContext* meta, MediaStream* stream) = 0;
		virtual ssize_t deserialize(MediaContext* meta, MediaStream* stream) = 0;
	};


	MediaStream();
	virtual ~MediaStream();

	virtual int open(void) = 0;
	virtual ssize_t read(uint8_t* rb, size_t sz) = 0;
	virtual ssize_t write(const uint8_t* wb, size_t sz) = 0;
	virtual int close() = 0;
};


class MediaFileStream : MediaStream{
public:
	MediaFileStream(const char* fname);
	virtual ~MediaFileStream();

	int open(void);
	ssize_t read(uint8_t* rb, size_t sz);
	ssize_t write(const uint8_t* wb,size_t sz);
	int close(void);

private:
	std::string* filename;
	int fd;
};

class MediaSocketStream : MediaStream {
public:
	MediaSocketStream();
	virtual ~MediaSocketStream();

	int open(void);
	ssize_t read(uint8_t* rb, size_t sz);
	ssize_t write(const uint8_t* wb,size_t sz);
	int close(void);

private:

};

class MediaBufferedStream : MediaStream {
public:
	MediaBufferedStream(size_t bsz);
	virtual ~MediaBufferedStream();

	int open(void);
	ssize_t read(uint8_t* rb, size_t sz);
	ssize_t write(const uint8_t* wb,size_t sz);
	int close(void);

private:

};

}

#endif /* MEDIASTREAM_H_ */
