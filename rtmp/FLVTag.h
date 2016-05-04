/*
 * FLVTag.h
 *
 *  Created on: Mar 16, 2016
 *      Author: innocentevil
 */

#ifndef FLVTAG_H_
#define FLVTAG_H_

#include <stdlib.h>
#include <stdint.h>

#include "core/MediaStream.h"
#include "core/Serialize.h"
#include "AMF0.h"

namespace MediaPipe {

class FLVTag : Serializable<void>{
public:
	FLVTag();
	virtual ~FLVTag();
	typedef enum {
		Audio = 8,
		Video = 9,
		Script = 18
	} TagType;

	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);

	TagType getType(void) const;
	void setType(TagType);
	size_t getSize(void) const;
	void setSize(size_t);
	uint32_t getTimestamp(void) const;
	void setTimestamp(uint32_t);
	uint32_t getStreamID(void) const;
	void setStreamID(uint32_t);
private:
	uint32_t t_stmp;			// decoding timestamp
	uint32_t stream_id;			// always 0
	uint32_t psize;				// payload(AudioTag + audiopayload or VideoTag + videopayload) size
	TagType  type;				// video / audio / datascript
};

class FLVVideoTag : public Serializable<FLVTag> {
public:
	FLVVideoTag();
	~FLVVideoTag();

	typedef enum {
		KEYFRAME = 1,					// for AVC, seekable frame
		INTERFRAME = 2,					// for AVC, non-seekable frame
		DISPOSABLE_INTERFRAME = 3,		// H.263 only
		GEN_KEYFRAME = 4,				// reserved for only server use
		VIDEO_INFO =5
	} FrameType;

	typedef enum {
		JPEG = 1,						// JPEG
		H263 = 2,						// H.263_Sorenson
		FlashSV_1 = 3,					// Screen Video
		VP6 = 4,						// On2 VP6
		VP6a = 5,						// On2 VP6 /w Alpha
		FlashSV_2 = 6,					// Screen Video Ver.2
		H264_AVC = 7					// H.264
	}CodecID;

	typedef enum {
		AVC_SeqHeader = 0,
		AVC_NALU = 1,
		AVC_EOS = 2
	}AVCPktType;

	ssize_t serialize(FLVTag* ctx, MediaStream* stream) ;
	ssize_t serialize(FLVTag* ctx, uint8_t* into);
	ssize_t deserialize(FLVTag* ctx, const MediaStream* stream);
	ssize_t deserialize(FLVTag* ctx, const uint8_t* from);


	AVCPktType getAvcType() const;
	void setAvcType(AVCPktType avcType);
	CodecID getCodecId() const;
	void setCodecId(CodecID codecId);
	FrameType getFrameType() const;
	void setFrameType(FrameType frameType);

private:
	FrameType frame_type;		// FrameType (Key Frame / Interframe and so on...)
	CodecID codec_id;			// Codec ID  (H.263 / H.264 and so on...)
	AVCPktType avc_type;		// AVC Chunk Specifier (Sequence header / nalu)
	uint32_t cts;				// composition time stamp which eventually (pts - dts) in millisec
};

class FLVAudioTag :  Serializable<FLVTag> {
public:
	FLVAudioTag();
	~FLVAudioTag();

	typedef enum {
		LPCM = 0,
		ADPCM = 1,
		MP3 = 2,
		LPCM_LE = 3,
		NELLYM_16K_MONO = 4,
		NELLYM_8K_MONO = 5,
		NELLYM = 6,
		G711A_LOGPCM = 7,
		G711u_LOGPCM = 8,
		AAC = 10,
		SPEEX = 11,
		MP3_8K = 14,
		DEV_SPECIFIC = 15
	}SoundFormat;

	typedef enum {
		R5_5KHz = 0,
		R11KHz = 1,
		R22KHz = 2,
		R44KHz = 3
	} SoundRate;

	typedef enum {
		SND_8b = 0,
		SND_16b = 1
	} SoundSize;

	typedef enum {
		MONO = 0,
		STEREO = 1
	}SoundType;

	typedef enum {
		AAC_SeqHeader = 0,
		AAC_RAW = 1,
	}AACPktType;

	ssize_t serialize(FLVTag* ctx, MediaStream* stream) ;
	ssize_t serialize(FLVTag* ctx, uint8_t* into);
	ssize_t deserialize(FLVTag* ctx, const MediaStream* stream);
	ssize_t deserialize(FLVTag* ctx, const uint8_t* from);


	void setSoundFormat(SoundFormat fmt);
	SoundFormat getSoundFormat(void) const;

	void setSoundRate(SoundRate rate);
	SoundRate getSoundRate(void) const;

	void setSoundSize(SoundSize sz);
	SoundSize getSoundSize(void) const;

	void setSoundType(SoundType type);
	SoundType getSoundType() const;

private:
	SoundFormat snd_format;
	SoundRate snd_rate;
	SoundType snd_type;
	SoundSize snd_sz;
	AACPktType aac_pkt_type;
};

class FLVDataScriptTag : Serializable<FLVTag>  {
public:
	FLVDataScriptTag();
	~FLVDataScriptTag();

	ssize_t serialize(FLVTag* ctx, MediaStream* stream) ;
	ssize_t serialize(FLVTag* ctx, uint8_t* into);
	ssize_t deserialize(FLVTag* ctx, const MediaStream* stream);
	ssize_t deserialize(FLVTag* ctx, const uint8_t* from);
};

}

#endif /* FLVTAG_H_ */
