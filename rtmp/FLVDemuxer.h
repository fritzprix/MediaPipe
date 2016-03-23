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
	class FLVPayloadEventHandler : public PayloadEventHandler<FLVTag>{
	public:
		FLVPayloadEventHandler(){};
		virtual ~FLVPayloadEventHandler() {};
		void onPayload(const Payload<FLVTag>* payload, const MediaStream* stream);
		void onPayload(const Payload<FLVTag>* payload, const uint8_t* buffer);
	};
	FLVDemuxer(MediaStream* input_stream);
	virtual ~FLVDemuxer();
	int setFLVPayloadEventHandler(FLVTag::TagType tagType,PayloadEventHandler<FLVTag>* handler);
	void parse();
private:
	FLVPayloadEventHandler* videoPayloadHandler;
	FLVPayloadEventHandler* audioPayloadHandler;
	FLVPayloadEventHandler* scriptPayloadHandler;
	const MediaStream* input_stream;
};

} /* namespace MediaPipe */

#endif /* FLVDEMUXER_H_ */
