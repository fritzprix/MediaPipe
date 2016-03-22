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
#include "AMF0.h"

namespace MediaPipe {

class FLVTag : MediaStream::Serializable, MediaContext {
public:
	FLVTag();
	virtual ~FLVTag();
	typedef enum {
		Audio = 8,
		Video = 9,
		Script = 18
	} TagType;

	ssize_t serialize(MediaContext* ctx, MediaStream* stream);
	ssize_t deserialize(MediaContext* ctx, MediaStream* stream);

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

class FLVPayload {
public:
	FLVPayload();
	virtual ~FLVPayload();
	virtual ssize_t readData(const MediaContext* ctx, uint8_t* data) = 0;
};

class FLVVideoTag :  MediaStream::Serializable , FLVPayload {
public:
	FLVVideoTag(size_t bsz);
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

	ssize_t serialize(MediaContext* ctx, MediaStream* stream);
	ssize_t deserialize(MediaContext* ctx, MediaStream* stream);

	ssize_t writeDataAVC(MediaContext* ctx, uint8_t* data, size_t sz, FrameType, CodecID, AVCPktType, uint32_t dts, uint32_t pts);
	ssize_t writeData(MediaContext* ctx, uint8_t* data, size_t sz, FrameType, CodecID, uint32_t dts, uint32_t pts);
	ssize_t writeData(MediaContext* ctx, uint8_t* data, size_t sz,  uint32_t dts, uint32_t pts);

	ssize_t readData(const MediaContext* ctx, uint8_t* data);

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
	uint8_t* obj_buffer;
	uint32_t cts;				// composition time stamp which eventually (pts - dts) in millisec
};

class FLVAudioTag :  MediaStream::Serializable, FLVPayload {
public:
	FLVAudioTag(size_t bsz);
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

	ssize_t serialize(MediaContext* ctx, MediaStream* stream);
	ssize_t deserialize(MediaContext* ctx, MediaStream* stream);

	/**
	 *  write audio data into flv chunk
	 *  @param ctx MediaContext(FLVTag here) to be updated by the audio raw data
	 *  @param data audio data buffer to be written
	 *  @param size size of audio data
	 *  @param
	 */
	ssize_t writeData(MediaContext* ctx, uint8_t* data, size_t sz, SoundFormat fmt, SoundRate rate, SoundSize smp_sz, SoundType snd_type, uint32_t timestamp);
	ssize_t writeDataAAC(MediaContext* ctx, uint8_t* data, size_t sz, SoundRate, SoundSize, SoundType, AACPktType, uint32_t timestamp);
	ssize_t writeData(MediaContext* ctx, uint8_t* data, size_t sz, uint32_t timestamp);

	ssize_t readData(const MediaContext* ctx, uint8_t* data);

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
	uint8_t* obj_buffer;
};

class FLVDataScriptTag : MediaStream::Serializable ,FLVPayload {
public:
	FLVDataScriptTag();
	~FLVDataScriptTag();

	ssize_t serialize(MediaContext* ctx, MediaStream* stream);
	ssize_t deserialize(MediaContext* ctx, MediaStream* stream);

	ssize_t readData(const MediaContext* ctx, uint8_t* data);
	AMF0* getScriptData(void);
private:
	AMF0 amf_script;
};

}

#endif /* FLVTAG_H_ */
