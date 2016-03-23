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

	template <class T>
	class Serializable {
	public:
		Serializable(){};
		virtual ~Serializable(){};
		virtual ssize_t serialize(T* ctx, MediaStream* stream) = 0;
		virtual ssize_t serialize(T* ctx, uint8_t* into) = 0;
		virtual ssize_t deserialize(T* ctx, const MediaStream* stream) = 0;
		virtual ssize_t deserialize(T* ctx, const uint8_t* from) = 0;
	};


	MediaStream();
	virtual ~MediaStream();

	virtual int open(void) = 0;
	virtual ssize_t read(uint8_t* rb, size_t sz) const = 0;
	virtual ssize_t write(const uint8_t* wb, size_t sz) = 0;
	virtual int close() = 0;
};

template <class T>
class Payload : MediaStream::Serializable<T> {
public:
	Payload(){};
	virtual ~Payload(){};
};

template <class T>
class PayloadEventHandler {
public:
	PayloadEventHandler() {};
	virtual ~PayloadEventHandler(){};
	virtual void onPayload(const Payload<T>* payload,const MediaStream* stream);
	virtual void onPayload(const Payload<T>* payload,const uint8_t* buffer);
};

template <class T>
class Unpackable {
public:
	Unpackable() {};
	virtual ~Unpackable() {};
	virtual Payload<T>* getPayload() = 0;
};

template <class T>
class Packable {
public:
	Packable() {};
	virtual ~Packable(){};
	virtual bool setPayload(const Payload<T>* payload) = 0;
};


class MediaFileStream : MediaStream{
public:
	MediaFileStream(const char* fname);
	virtual ~MediaFileStream();

	int open(void);
	ssize_t read(uint8_t* rb, size_t sz) const;
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
	ssize_t read(uint8_t* rb, size_t sz) const;
	ssize_t write(const uint8_t* wb,size_t sz);
	int close(void);

private:

};

class MediaBufferedStream : MediaStream {
public:
	MediaBufferedStream(size_t bsz);
	virtual ~MediaBufferedStream();

	int open(void);
	ssize_t read(uint8_t* rb, size_t sz) const;
	ssize_t write(const uint8_t* wb,size_t sz);
	int close(void);

private:

};

}

#endif /* MEDIASTREAM_H_ */
