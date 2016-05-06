/*
 * Demuxer.h
 *
 *  Created on: May 5, 2016
 *      Author: innocentevil
 */

#ifndef CORE_DEMUXER_H_
#define CORE_DEMUXER_H_

namespace MediaPipe {

template<class T>
class DemuxerChunkHandler {
public:
	DemuxerChunkHandler(){};
	virtual ~DemuxerChunkHandler(){};
	virtual bool onChunkHandle(T* header, MediaStream* stream) = 0;
};

template<class T>
class Demuxer {
public:
	const static int VIDEO = 1;
	const static int ADUIO = 2;
	const static int META = 4;

	typedef int ChunkType;

	Demuxer(){};
	virtual ~Demuxer(){};
	virtual void setChunkHandler(DemuxerChunkHandler<T>* handler,ChunkType type) = 0;
	virtual bool demux() = 0;
};

} /* namespace MediaPipe */

#endif /* CORE_DEMUXER_H_ */
