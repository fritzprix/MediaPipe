/*
 * FLVDumper.h
 *
 *  Created on: May 5, 2016
 *      Author: innocentevil
 */

#ifndef RTMP_FLVDUMPER_H_
#define RTMP_FLVDUMPER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "core/Serialize.h"
#include "core/MediaStream.h"
#include "core/Demuxer.h"
#include "FLVTag.h"

namespace MediaPipe {

class FLVDumper : public Serializable<FLVTag> , public DemuxerChunkHandler<FLVTag> {
public:
	FLVDumper(void* buffer);
	FLVDumper(void* buffer, MediaStream* to);
	virtual ~FLVDumper();
	ssize_t serialize(FLVTag* ctx, MediaStream* stream);
	ssize_t serialize(FLVTag* ctx, uint8_t* into);
	ssize_t deserialize(FLVTag* ctx, const MediaStream* stream);
	ssize_t deserialize(FLVTag* ctx, const uint8_t* from);

	bool onChunkHandle(FLVTag* header, MediaStream* stream);
private:
	void* rbuffer;
	MediaStream* out;
};

} /* namespace MediaPipe */

#endif /* RTMP_FLVDUMPER_H_ */
