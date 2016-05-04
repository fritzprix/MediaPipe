/*
 * Serialize.h
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#ifndef CORE_SERIALIZE_SERIALIZE_H_
#define CORE_SERIALIZE_SERIALIZE_H_

#include "MediaStream.h"

namespace MediaPipe {

template<class T>
class Serializable {
public:
	Serializable() {};
	virtual ~Serializable() {};
	virtual ssize_t serialize(T* ctx, MediaStream* stream) = 0;
	virtual ssize_t serialize(T* ctx, uint8_t* into) = 0;
	virtual ssize_t deserialize(T* ctx, const MediaStream* stream) = 0;
	virtual ssize_t deserialize(T* ctx, const uint8_t* from) = 0;
};


}

#endif /* CORE_SERIALIZE_SERIALIZE_H_ */
