/*
 * FLVDemuxer.cpp
 *
 *  Created on: Mar 18, 2016
 *      Author: innocentevil
 */

#include "FLVDemuxer.h"

namespace MediaPipe {

FLVDemuxer::FLVDemuxer(MediaStream* stream) {
	videoPayloadHandler = audioPayloadHandler = scriptPayloadHandler = NULL;
	input_stream = stream;
}

FLVDemuxer::~FLVDemuxer() {
	input_stream = NULL;
}

int FLVDemuxer::setFLVPayloadEventHandler(FLVTag::TagType tagType, PayloadEventHandler<FLVTag>* handler) {
	switch(tagType) {
	case FLVTag::Audio:
		videoPayloadHandler = (FLVPayloadEventHandler*) handler;
		break;
	case FLVTag::Video:
		audioPayloadHandler = (FLVPayloadEventHandler*) handler;
		break;
	case FLVTag::Script:
		scriptPayloadHandler = (FLVPayloadEventHandler*) handler;
		break;
	}
}

void FLVDemuxer::parse() {
	FLVTag tag;
	tag.deserialize(NULL, input_stream);
}


} /* namespace MediaPipe */
