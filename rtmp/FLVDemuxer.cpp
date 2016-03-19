/*
 * FLVDemuxer.cpp
 *
 *  Created on: Mar 18, 2016
 *      Author: innocentevil
 */

#include "FLVDemuxer.h"

namespace MediaPipe {

FLVDemuxer::FLVDemuxer(MediaStream* stream) {
	audio_handler = video_handler = script_handler = NULL;
	input_stream = stream;
}

FLVDemuxer::~FLVDemuxer() {
	input_stream = NULL;
}

int FLVDemuxer::setFLVTagHandler(FLVTag::TagType tagType,
		FLVTagHandler* handler) {
}

void FLVDemuxer::parse(size_t vbuf_sz, size_t abuf_sz) {
	FLVTag tag = FLVTag();
	FLVVideoTag vtag = FLVVideoTag(vbuf_sz);
	FLVAudioTag atag = FLVAudioTag(abuf_sz);

}

FLVDemuxer::FLVTagHandler::FLVTagHandler() {
}

FLVDemuxer::FLVTagHandler::~FLVTagHandler() {
}

} /* namespace MediaPipe */
