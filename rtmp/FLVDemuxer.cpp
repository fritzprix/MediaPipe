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

}

void FLVDemuxer::parse() {
	FLVTag tag;
}


} /* namespace MediaPipe */
