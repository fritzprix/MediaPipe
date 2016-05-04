/*
 * FLVDemuxer.cpp
 *
 *  Created on: Mar 18, 2016
 *      Author: innocentevil
 */

#include "FLVDemuxer.h"

namespace MediaPipe {

FLVDemuxer::FLVDemuxer(MediaStream* stream) {
	input_stream = stream;
}

FLVDemuxer::~FLVDemuxer() {
	input_stream = NULL;
}

int FLVDemuxer::parse() {
	FLVTag commonTag;
	FLVVideoTag videoTag;
	FLVAudioTag audioTag;
	FLVDataScriptTag scriptTag;
	while(commonTag.deserialize(NULL, input_stream) > 0)
	{
		switch(commonTag.getType()) {
		case FLVTag::Audio:
			if(audioTag.deserialize(&commonTag, input_stream) < 0)
				return -1;
			break;
		case FLVTag::Video:
			if(videoTag.deserialize(&commonTag, input_stream) < 0)
				return -1;
			break;
		case FLVTag::Script:
			if(scriptTag.deserialize(&commonTag, input_stream) < 0)
				return -1;
			break;
		}
	}
	return 0;
}


} /* namespace MediaPipe */
