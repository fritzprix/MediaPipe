/*
 * FLVDumper.cpp
 *
 *  Created on: May 5, 2016
 *      Author: innocentevil
 */

#include "FLVDumper.h"
#include <string.h>

namespace MediaPipe {


FLVDumper::FLVDumper(void* buffer)
{
	rbuffer = buffer;
	out = NULL;
}

FLVDumper::FLVDumper(void* buffer, MediaStream* to)
{
	rbuffer = buffer;
	out = to;
}

FLVDumper::~FLVDumper()
{
	rbuffer = NULL;
	out = NULL;
}

ssize_t FLVDumper::serialize(FLVTag* ctx, MediaStream* stream)
{
	return -1;
}
ssize_t FLVDumper::serialize(FLVTag* ctx, uint8_t* into)
{
	return -1;
}
ssize_t FLVDumper::deserialize(FLVTag* ctx, const MediaStream* stream)
{
	if(!ctx || !stream)
		return -1;
	ssize_t sz;
	if((sz = stream->read(rbuffer, ctx->getSize())) < 0)
	{
		return sz;
	}
	if(out)
	{
		out->write(rbuffer,sz);
	}
	return sz;
}

ssize_t FLVDumper::deserialize(FLVTag* ctx, const uint8_t* from)
{
	if(!ctx || !from)
		return -1;
	size_t sz = ctx->getSize();
	if(!memcpy(rbuffer,from, sz))
		return -1;
	if(out)
	{
		out->write(rbuffer,sz);
	}
	return sz;
}

bool FLVDumper::onChunkHandle(FLVTag* header, MediaStream* stream)
{
	size_t sz = header->getSize();
	ssize_t res;
	if((res = stream->read(rbuffer, sz)) < 0)
		return false;
	out->write(rbuffer, sz);
	return true;
}


} /* namespace MediaPipe */
