/*
 * FLVDemuxer.cpp
 *
 *  Created on: Mar 18, 2016
 *      Author: innocentevil
 */

#include "FLVDemuxer.h"
#include "string.h"

typedef struct {
	uint8_t          sig[3];
	uint8_t          ver;
#define VIDEO_FLAG   ((uint8_t) 1)
#define AUDIO_FLAG   ((uint8_t) 1 << 2)
	uint8_t          flag;
	uint32_t         offset;
	uint32_t         prev_sz;
}__attribute__((packed)) flv_header;

const uint8_t FLV_SIG[] = {
		'F',
		'L',
		'V'
};

namespace MediaPipe {

FLVDemuxer::FLVDemuxer(MediaStream* stream) {
	input_stream = stream;
	vhandler = ahandler = mhandler = NULL;
}

FLVDemuxer::~FLVDemuxer() {
	input_stream = NULL;
	vhandler = ahandler = mhandler = NULL;
}

void FLVDemuxer::setChunkHandler(DemuxerChunkHandler<FLVTag>* handler, Demuxer::ChunkType type)
{
	if(type & Demuxer::VIDEO) {
		vhandler = handler;
	}
	if(type & Demuxer::ADUIO) {
		ahandler = handler;
	}
	if(type & Demuxer::META) {
		mhandler = handler;
	}
}


bool FLVDemuxer::demux() {
	FLVCommonTag commonTag;
	FLVVideoTag videoTag;
	FLVAudioTag audioTag;
	FLVDataScriptTag scriptTag;
	ssize_t res;
	uint32_t prev_sz;
	size_t payload_sz;
	flv_header header;
	if((res = input_stream->read(&header, sizeof(flv_header))) < 0)
		return res;
	if(memcmp(FLV_SIG,header.sig,3) && header.prev_sz)
		return -1;
	while(commonTag.deserialize(NULL, input_stream) > 0)
	{
		switch(commonTag.getType()) {
		case FLVTag::Audio:
			::printf("Audio Type : ");
			if(audioTag.deserialize(&commonTag, input_stream) < 0)
				return false;
			if((payload_sz = audioTag.getSize()) == 0)
				return true;
			::printf("Payload Size(%lu)\n",payload_sz);
			if(ahandler) {
				if(ahandler->onChunkHandle(&audioTag,(MediaStream*) input_stream))
					break;
			}
			if((res = input_stream->skip(audioTag.getSize())) < 0)
				return false;
			break;
		case FLVTag::Video:
			::printf("Video Type : ");
			if(videoTag.deserialize(&commonTag, input_stream) < 0)
				return false;
			if((payload_sz = videoTag.getSize()) == 0)
				return true;
			::printf("Payload Size(%lu)\n",payload_sz);
			if(vhandler)
			{
				if(vhandler->onChunkHandle(&videoTag,(MediaStream*) input_stream))
					break;
			}
			if((res = input_stream->skip(videoTag.getSize())) < 0)
				return false;
			break;
		case FLVTag::Script:
			::printf("Script Type : ");
			if(scriptTag.deserialize(&commonTag, input_stream) < 0)
				return false;
			if((payload_sz = scriptTag.getSize()) == 0)
				return true;
			::printf("Payload Size(%lu)\n",payload_sz);
			if(mhandler)
			{
				if(mhandler->onChunkHandle(&scriptTag, (MediaStream*) input_stream))
					break;
			}
			if((res = input_stream->skip(scriptTag.getSize())) < 0)
				return false;
			break;
		default:
			::printf("unexpected type!\n");
			return false;
		}
		input_stream->read(&prev_sz,sizeof(uint32_t));
	}
	return true;
}


} /* namespace MediaPipe */
