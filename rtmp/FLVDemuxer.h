/*
 * FLVDemuxer.h
 *
 *  Created on: Mar 18, 2016
 *      Author: innocentevil
 */

#include "core/MediaStream.h"
#include "core/Demuxer.h"
#include "FLVTag.h"

#ifndef FLVDEMUXER_H_
#define FLVDEMUXER_H_

namespace MediaPipe {


class FLVDemuxer : public Demuxer<FLVTag>{
public:
	FLVDemuxer(MediaStream* input_stream);
	virtual ~FLVDemuxer();
	void setChunkHandler(DemuxerChunkHandler<FLVTag>* handler, ChunkType type);
	bool demux();
private:
	const MediaStream* input_stream;
	DemuxerChunkHandler<FLVTag>* vhandler;
	DemuxerChunkHandler<FLVTag>* ahandler;
	DemuxerChunkHandler<FLVTag>* mhandler;
};

} /* namespace MediaPipe */

#endif /* FLVDEMUXER_H_ */
