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
	class FLVTagHandler {
	public:
		FLVTagHandler();
		virtual ~FLVTagHandler();
		virtual void onTagHandle(FLVTag* flvTag, FLVPayload* payload) = 0;
	};
	FLVDemuxer(MediaStream* input_stream);
	virtual ~FLVDemuxer();
	int setFLVTagHandler(FLVTag::TagType tagType, FLVTagHandler* handler);
	void parse(size_t vb_sz = (1 << 20), size_t ab_sz = (1 << 12));
private:
	FLVTagHandler* video_handler;
	FLVTagHandler* audio_handler;
	FLVTagHandler* script_handler;

	const MediaStream* input_stream;
};

} /* namespace MediaPipe */

#endif /* FLVDEMUXER_H_ */
