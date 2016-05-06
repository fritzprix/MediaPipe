/*
 * FLVTestUnit.h
 *
 *  Created on: May 1, 2016
 *      Author: innocentevil
 */

#ifndef RTMP_FLVTESTUNIT_H_
#define RTMP_FLVTESTUNIT_H_

#include "core/MediaStream.h"
#include "core/stream/MediaFileStream.h"
#include "FLVDemuxer.h"
#include "FLVDumper.h"

namespace MediaPipe {

class FLVTestUnit {
public:
	FLVTestUnit();
	virtual ~FLVTestUnit();
	bool performTest();
private:
	MediaFileStream* filestream;
	MediaFileStream* dumpstream;
	FLVDemuxer* demuxer;
	FLVDumper* dumper;
	uint32_t* buffer;
};

} /* namespace MediaPipe */

#endif /* RTMP_FLVTESTUNIT_H_ */
