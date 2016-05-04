/*
 * FLVDemuxer.h
 *
 *  Created on: Mar 18, 2016
 *      Author: innocentevil
 */

#include "core/MediaStream.h"
#include "FLVTag.h"

#ifndef FLVDEMUXER_H_
#define FLVDEMUXER_H_

namespace MediaPipe {


class FLVDemuxer {
public:
	FLVDemuxer(MediaStream* input_stream);
	virtual ~FLVDemuxer();
	int parse();
private:
	const MediaStream* input_stream;
};

} /* namespace MediaPipe */

#endif /* FLVDEMUXER_H_ */
