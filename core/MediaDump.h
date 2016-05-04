/*
 * MediaDump.h
 *
 *  Created on: May 5, 2016
 *      Author: innocentevil
 */

#ifndef CORE_MEDIADUMP_H_
#define CORE_MEDIADUMP_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "MediaStream.h"
#include "Serialize.h"

namespace MediaPipe {

class MediaDump : public Serializable<void>{
public:
	MediaDump();
	virtual ~MediaDump();
    ssize_t serialize(void* ctx, MediaStream* stream);
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);

};

} /* namespace MediaPipe */

#endif /* CORE_MEDIADUMP_H_ */
