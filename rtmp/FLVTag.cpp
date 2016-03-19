/*
 * FLVTag.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: innocentevil
 */

#include "FLVTag.h"
#include "mpipe.h"


#include <stdio.h>
#include <string.h>



/**********************************************************************************/
/*******   start of type definition to be used for parsing flv stream   ***********/
/**********************************************************************************/
typedef struct {
	uint8_t 	_[3];
} __attribute__((packed)) uint24_t;

typedef struct {
	uint8_t 	type;
	uint24_t	data_sz;
	uint24_t	time_stmp;
	uint8_t		time_stmp_ex;
	uint24_t	stream_id;
} __attribute__((packed)) flv_tag_t;

typedef struct {
#define AAC_SEQ_HEADER 	((uint8_t) 0)
#define AAC_RAW			((uint8_t) 1)
	uint8_t		aac_pkt_type;
}__attribute__((packed)) flv_aac_audio_t;

typedef struct {
	uint8_t					flags;
	union {
		flv_aac_audio_t		aac_ex;
		uint8_t				_data;
	}__attribute__((packed)) ;
} __attribute__((packed)) flv_audio_t;

typedef struct {
	uint8_t					pkt_type;
	uint24_t				cps_time;
} __attribute__((packed)) flv_video_avc_t;

typedef struct {
	uint8_t 				flags;
	union {
		flv_video_avc_t 		avc_ex;
		uint8_t					_data[4];
	}__attribute__((packed));
} __attribute__((packed)) flv_video_t;


/*
 * forward decl. of flv script object types
 */
typedef struct flv_script_data_obj  flv_script_data_obj_t;
typedef struct flv_script_data_val  flv_script_data_val_t;
typedef struct flv_script_data_str  flv_script_data_str_t;
typedef struct flv_script_data_date  flv_script_data_date_t;
typedef struct flv_script_data_lstr  flv_script_data_lstr_t;
typedef struct flv_script_data_vlen	 flv_script_data_vlen_t;
typedef struct flv_script_data_obj_end  flv_script_data_obj_ent_t;
typedef struct flv_script_emca_array  flv_script_emca_array_t;
typedef struct flv_script_strict_array flv_script_strict_array_t;


struct flv_script_data_obj_end{
	uint24_t					marker;
} __attribute__((packed));


/**
 *  SCRIPTDATASTRING
 */
struct flv_script_data_str {
	uint16_t					str_len;
	char						str_data;
} __attribute__((packed));

/*
 * SCRIPTDATALONGSTRING
 */
struct flv_script_data_lstr {
	uint32_t		str_len;
	char			str_data;
} __attribute__((packed));

struct flv_script_data_date {
	double 			datetime;
	uint16_t		utc_offset;
} __attribute__((packed));

struct flv_script_obj_property {
	flv_script_data_str_t		prop_name;
	uint8_t						prop_value;	// begining of SCRIPTDATAVALUE
};

/**
 * SCRIPTDATAOBJECT
 */
struct flv_script_data_obj {
	flv_script_obj_property		script_obj_property_start;	// array of properties until end marker
} __attribute__((packed));

struct flv_script_emca_array {
	uint32_t					approx_sz;
	uint8_t						scrit_obj_property_start; // beginning of SCRIPTDATAOBJECTPROPERY
} __attribute__((packed));

struct flv_script_strict_array {
	uint32_t					size;
	uint8_t						script_data_val_start;
};

/**
 *  SCRIPTDATAVALUE
 */
struct flv_script_data_val {
#define SCRIPT_DATA_NUMBER			((uint8_t) 0)
#define SCRIPT_DATA_BOOL			((uint8_t) 1)
#define SCRIPT_DATA_STRING			((uint8_t) 2)
#define SCRIPT_DATA_OBJECT			((uint8_t) 3)
#define SCRIPT_DATA_MOVIE_CLIP		((uint8_t) 4)
#define SCRIPT_DATA_NULL			((uint8_t) 5)
#define SCRIPT_DATA_REF				((uint8_t) 7)
#define SCRIPT_DATA_ECMA_AR			((uint8_t) 8)
#define SCRIPT_DATA_STRICT_AR		((uint8_t) 10)
#define SCRIPT_DATE_TYPE			((uint8_t) 11)
#define SCRIPT_LONG_STRING			((uint8_t) 12)
	uint8_t							type;
	union {
		double						number;
		uint8_t						boolean;
		flv_script_data_str_t		str;
		flv_script_data_obj_t		objs;
		flv_script_data_str_t		movie_clip_path;
		uint16_t					ref;
		flv_script_emca_array_t		emca_arr;
		flv_script_strict_array_t 	strict_arr;
		flv_script_data_date		date;
		flv_script_data_lstr_t		lstr;
	} val;
} __attribute__((packed));


typedef struct {
	flv_script_data_val_t			name;
} __attribute__((packed)) flv_script_t;

/**********************************************************************************/
/*******     end of type definition to be used for parsing flv stream   ***********/
/**********************************************************************************/


static uint32_t __bswap_u24_to_u32(uint24_t* u);
static void __bswap_u32_to_u24(uint32_t s, uint24_t* u);

namespace MediaPipe {

FLVTag::FLVTag() {
	type = Video;
	t_stmp = 0;
	psize = 0;
	stream_id = 0;
}

FLVTag::~FLVTag() { }

ssize_t FLVTag::serialize(MediaContext* ctx,MediaStream* stream) {
	flv_tag_t tag;
	if(!stream)
	{
		return -1;
	}

	tag.type = this->type;
	tag.time_stmp_ex = this->t_stmp >> 24;
	__bswap_u32_to_u24(this->psize, &tag.data_sz);
	__bswap_u32_to_u24(this->t_stmp, &tag.time_stmp);
	__bswap_u32_to_u24(this->stream_id, &tag.stream_id);

	return stream->write((const uint8_t*) &tag, sizeof(flv_tag_t));
}

ssize_t FLVTag::deserialize(MediaContext* ctx, MediaStream* stream) {
	flv_tag_t tag;
	if(!stream)
	{
		return -1;
	}

	ssize_t res = stream->read((uint8_t*) &tag,sizeof(flv_tag_t));
	if(res > 0)
	{
		type = (TagType) tag.type;
		t_stmp = (tag.time_stmp_ex << 24) | __bswap_u24_to_u32(&tag.time_stmp);
		psize = __bswap_u24_to_u32(&tag.data_sz);
		stream_id = __bswap_u24_to_u32(&tag.stream_id);
	}
	return res;
}

FLVTag::TagType FLVTag::getType(void) const{
	return type;
}

void FLVTag::setType(TagType type) {
	this->type = type;
}

size_t FLVTag::getSize(void) const {
	return psize;
}

void FLVTag::setSize(size_t size) {
	this->psize = size;
}

uint32_t FLVTag::getTimestamp(void) const {
	return t_stmp;
}

void FLVTag::setTimestamp(uint32_t timestamp) {
	this->t_stmp = timestamp;
}

uint32_t FLVTag::getStreamID(void) const {
	return stream_id;
}

void FLVTag::setStreamID(uint32_t stream_id) {
	this->stream_id = stream_id;
}

FLVAudioTag::FLVAudioTag(size_t bsz) {

	this->snd_format = AAC;
	this->snd_rate = R44KHz;
	this->snd_sz = SND_16b;
	this->snd_type = STEREO;
	this->aac_pkt_type = AAC_SeqHeader;

	/** obj_buffer is actual byte stream of FLVAudioTag but there is gap between audio header and payload
	 *  for convenience.
	 *   AAC AudioTAG
	 *    | ======== flv_audio_t (2 bytes)  ========== | ======  payload ========  |
	 *
	 *   non-AAC AudioTAG
	 *    |  flv_audio_t.flag (1 byte)  | gap (1 byte) | ======  payload ========  |
	 *
	 *
	 */

	this->obj_buffer = new uint8_t[bsz + sizeof(flv_audio_t)];

	if(!this->obj_buffer)
	{
		perror("out of memory !! \n");
		exit(-1);
	}
}

FLVAudioTag::~FLVAudioTag() {
	delete this->obj_buffer;
}


ssize_t FLVAudioTag::serialize(MediaContext* ctx,MediaStream* stream) {
	if(!stream || !ctx)
	{
		::perror("null pointer argument !!");
		::exit(-1);
	}

	FLVTag* tag = (FLVTag*) ctx;
	flv_audio_t* audio_tag = (flv_audio_t*) obj_buffer;
	uint8_t audio_tag_sz = 0;
	audio_tag->flags = (this->snd_format << 4) | \
						(this->snd_rate << 2) | \
						(this->snd_sz << 1) | \
						(this->snd_type);
	audio_tag_sz += sizeof(audio_tag->flags);
	if(this->snd_format == AAC)
	{
		// if audio tag is AAC type
		// aac pkt type should be written into stream
		audio_tag->aac_ex.aac_pkt_type = this->aac_pkt_type;
		audio_tag_sz += sizeof(audio_tag->aac_ex);
		if(stream->write((uint8_t*) audio_tag, audio_tag_sz) < 0)
		{
			return -1;
		}
	}
	else
	{
		// write audio tag (header)  into stream
		if(stream->write((uint8_t*) audio_tag, audio_tag_sz) < 0)
		{
			return -1;
		}
	}
	return stream->write((uint8_t*) &audio_tag[1], tag->getSize() - audio_tag_sz) + audio_tag_sz;
}

ssize_t FLVAudioTag::deserialize(MediaContext* ctx,MediaStream* stream) {
	if(!stream || !ctx)
	{
		::perror("null pointer argument !!");
		::exit(-1);
	}

	FLVTag* tag = (FLVTag* ) ctx;
	uint8_t audio_tag_sz = 0;
	uint8_t pread_offset = 0;
	flv_audio_t* audio_tag = (flv_audio_t*) obj_buffer;
	if(this->snd_format == AAC)
	{
		// if AAC chunk read stream with full size of FLVAudioTag (2 bytes)
		if(stream->read((uint8_t*) audio_tag, sizeof(flv_audio_t)) < 0)
		{
			return -1;
		}
		if((audio_tag->flags >> 4) != this->snd_format)
		{
			// sound format inconsistent!!
			// means this audio chunk is not AAC.
			// and therefore last octet is actually first octet of payload
			// so it should be copied into payload position
			*((uint8_t*) &audio_tag[1]) = audio_tag->aac_ex.aac_pkt_type;
			pread_offset += sizeof(audio_tag->aac_ex);
			audio_tag_sz += sizeof(audio_tag->flags);
		}
		else
		{
			// sound format consistent
			// means this chunk is AAC
			// so fill out AACPktType
			aac_pkt_type = (AACPktType) audio_tag->aac_ex.aac_pkt_type;
			audio_tag_sz += sizeof(flv_audio_t);
		}

	}
	else
	{
		// if non-AAC chunk read stream only single byte
		if(stream->read((uint8_t*) audio_tag, sizeof(audio_tag->flags)) < 0)
		{
			return -1;
		}
		audio_tag_sz = sizeof(audio_tag->flags);
	}
	// update audio stream information
	this->snd_format = (SoundFormat) (audio_tag->flags >> 4);
	this->snd_rate = (SoundRate)((audio_tag->flags & 0xF) >> 2);
	this->snd_sz = (SoundSize) ((audio_tag->flags & 0x6) >> 1);
	this->snd_type = (SoundType) (audio_tag->flags & 0x1);

	return stream->read(((uint8_t*) &audio_tag[1]) + pread_offset, tag->getSize() - audio_tag_sz) + audio_tag_sz;
}

void FLVAudioTag::setSoundFormat(SoundFormat fmt) {
	snd_format = fmt;
}

FLVAudioTag::SoundFormat FLVAudioTag::getSoundFormat(void) const {
	return snd_format;
}

void FLVAudioTag::setSoundRate(SoundRate rate) {
	snd_rate = rate;
}
FLVAudioTag::SoundRate FLVAudioTag::getSoundRate(void) const {
	return snd_rate;
}


void FLVAudioTag::setSoundSize(SoundSize sz) {
	snd_sz = sz;
}

FLVAudioTag::SoundSize FLVAudioTag::getSoundSize(void) const {
	return snd_sz;
}

void FLVAudioTag::setSoundType(SoundType type) {
	snd_type = type;
}

ssize_t FLVAudioTag::readData(const MediaContext* ctx, uint8_t* data) {
	if(!data || !ctx)
		return -1;
	const FLVTag* tag = (const FLVTag* ) ctx;
	size_t psz = 0, hsz = 0;
	flv_audio_t* audio_tag = (flv_audio_t*) obj_buffer;
	/*
	 * get payload size in bytes
	 * payload size = size of audio_tag + size of raw audio data in bytes
	 */
	psz = tag->getSize();

	// calculate header size depends on sound format
	if(snd_format == AAC)
	{
		hsz = sizeof(flv_audio_t);
	}
	else
	{
		hsz = sizeof(audio_tag->flags);
	}

	::memcpy(data, &audio_tag[1], psz - hsz);
	return psz - hsz;
}

FLVAudioTag::SoundType FLVAudioTag::getSoundType() const {
	return snd_type;
}


ssize_t FLVAudioTag::writeData(MediaContext* ctx, uint8_t* payload, size_t sz, SoundFormat format,
		SoundRate rate, SoundSize smpsz, SoundType type, uint32_t timestamp) {
	if(!payload)
	{
		::perror("null pointer argument !\n");
		return -1;
	}
	snd_format = format;
	snd_sz = smpsz;
	snd_rate = rate;
	snd_type = type;

	FLVTag* flvTag = (FLVTag*) ctx;
	flv_audio_t* audio_tag = (flv_audio_t*) obj_buffer;
	if(format == AAC)
	{
		flvTag->setSize(sz + sizeof(flv_audio_t));
	}
	else
	{
		flvTag->setSize(sz + sizeof(audio_tag->flags));
	}

	flvTag->setType(FLVTag::Audio);
	flvTag->setTimestamp(timestamp);

	::memcpy(&audio_tag[1], payload, sz);
	return sz;
}

ssize_t FLVAudioTag::writeData(MediaContext* ctx, uint8_t* payload, size_t sz, uint32_t timestamp) {
	if(!payload)
	{
		::perror("null pointer argument !\n");
		return -1;
	}
	FLVTag* flvTag = (FLVTag*) ctx;
	flv_audio_t* audio_tag = (flv_audio_t*) obj_buffer;
	if(this->snd_format == AAC)
	{
		flvTag->setSize(sz + sizeof(flv_audio_t));
	}
	else
	{
		flvTag->setSize(sz + sizeof(audio_tag->flags));
	}
	flvTag->setType(FLVTag::Audio);
	flvTag->setTimestamp(timestamp);
	::memcpy(&audio_tag[1], payload, sz);
	return sz;
}

ssize_t FLVAudioTag::writeDataAAC(MediaContext* ctx, uint8_t* payload, size_t sz, SoundRate snd_rate,
		SoundSize smpl_sz, SoundType snd_type, AACPktType aac_ex, uint32_t timestamp) {
	if(!payload)
	{
		::perror("null pointer argument !\n");
		return -1;
	}
	FLVTag* flvTag = (FLVTag*) ctx;
	flvTag->setSize(sz + sizeof(flv_audio_t));
	flvTag->setType(FLVTag::Audio);
	flvTag->setTimestamp(timestamp);
	snd_format = AAC;
	this->snd_rate = snd_rate;
	this->snd_sz = smpl_sz;
	this->snd_type = snd_type;
	this->aac_pkt_type = aac_ex;

	flv_audio_t* audio_tag = (flv_audio_t*) obj_buffer;
	::memcpy(&audio_tag[1], payload, sz);
	return sz;
}


FLVVideoTag::FLVVideoTag(size_t bsz) {
	frame_type = KEYFRAME;
	codec_id = H264_AVC;
	avc_type = AVC_SeqHeader;
	cts = 0;
	this->obj_buffer = new uint8_t[bsz];

	if(!this->obj_buffer)
	{
		::perror("out of memory !! \n");
		::exit(-1);
	}
}

FLVVideoTag::~FLVVideoTag() {
	delete this->obj_buffer;
}

ssize_t FLVVideoTag::serialize(MediaContext* ctx, MediaStream* stream) {
	if(!stream || !ctx)
	{
		::perror("null pointer argument !\n");
		::exit(-1);
	}

	FLVTag* tag = (FLVTag*) ctx;
	flv_video_t* video_tag = (flv_video_t*) obj_buffer;
	uint8_t video_tag_sz = 0;
	video_tag->flags = ((frame_type << 4) | (codec_id & 0xF));
	video_tag_sz += sizeof(video_tag->flags);

	if(codec_id == H264_AVC)
	{
		// if h.264 chunk fill out AVC extended tag
		__bswap_u32_to_u24(cts, &video_tag->avc_ex.cps_time);   // cts is defined (pts - dts)
		video_tag->avc_ex.pkt_type = avc_type;
		video_tag_sz += sizeof(video_tag->avc_ex);

		if(stream->write((const uint8_t*) video_tag, video_tag_sz) < 0)
		{
			return -1;
		}
	}
	else
	{
		if(stream->write((const uint8_t*) video_tag, video_tag_sz) < 0)
		{
			return -1;
		}
	}

	return stream->write((const uint8_t*) &video_tag[1], (tag->getSize() - video_tag_sz));
}

ssize_t FLVVideoTag::deserialize(MediaContext* ctx, MediaStream* stream) {
	if(!stream || !ctx)
	{
		::perror("null pointer argument !\n");
		::exit(-1);
	}

	FLVTag* tag = (FLVTag*) ctx;
	flv_video_t* video_tag = (flv_video_t*) obj_buffer;
	uint8_t video_tag_sz = 0;
	uint8_t pread_offset = 0;
	if(codec_id == H264_AVC)
	{
		if(stream->read((uint8_t*) video_tag, sizeof(flv_video_t)) < 0)
		{
			return -1;
		}

		if((video_tag->flags & 0xF) != codec_id)
		{
			// codec is inconsistent!!
			// avc packet header part is actually video chunk payload
			// so copy them into payload
			::memcpy(&video_tag[1], &video_tag->avc_ex, sizeof(video_tag->avc_ex));
			video_tag_sz += sizeof(video_tag->flags);
			pread_offset += sizeof(video_tag->avc_ex);
		}
		else
		{
			// if codec is consistent
			// that means this chunk is H264
			// so fill avc packet header part
			cts = __bswap_u24_to_u32(&video_tag->avc_ex.cps_time);
			avc_type = (AVCPktType) video_tag->avc_ex.pkt_type;
			video_tag_sz += sizeof(video_tag);
		}
	}
	else
	{
		if(stream->read((uint8_t*) video_tag, sizeof(video_tag->flags)) < 0)
		{
			return -1;
		}
	}
	frame_type = (FrameType) (video_tag->flags >> 4);
	codec_id = (CodecID) (video_tag->flags & 0xF);

	return stream->read(((uint8_t*) &video_tag[1] + pread_offset), tag->getSize() - video_tag_sz );
}

ssize_t FLVVideoTag::writeDataAVC(MediaContext* ctx, uint8_t* data, size_t sz, FrameType type,
		CodecID codec, AVCPktType avc_type, uint32_t dts, uint32_t pts) {
	if(!data || !ctx || !sz)
	{
		::perror("null pointer argument");
		::exit(-1);
	}
	FLVTag* tag = (FLVTag*) ctx;
	flv_video_t* video_tag = (flv_video_t*) obj_buffer;
	tag->setSize(sz + sizeof(flv_video_t));
	tag->setTimestamp(dts);
	tag->setType(FLVTag::Video);

	this->frame_type = type;
	this->codec_id = codec;
	this->avc_type = avc_type;
	this->cts = pts - dts;

	::memcpy(&video_tag[1], data, sz);
	return sz;
}

ssize_t FLVVideoTag::writeData(MediaContext* ctx, uint8_t* data, size_t sz, FrameType type,
		CodecID codec, uint32_t dts, uint32_t pts) {
	if(!data || !ctx || !sz)
	{
		::perror("null pointer argument");
		::exit(-1);
	}

	FLVTag* tag = (FLVTag*) ctx;
	flv_video_t* video_tag = (flv_video_t*) obj_buffer;
	if(codec == H264_AVC)
	{
		tag->setSize(sz + sizeof(video_tag));
		// TODO: We Improve here -> we can check bit pattern to decide whether NALU or not
		this->avc_type = AVC_NALU;
	}
	else
	{
		tag->setSize(sz + sizeof(video_tag->flags));
	}
	tag->setTimestamp(dts);
	tag->setType(FLVTag::Video);

	this->frame_type = type;
	this->codec_id = codec;
	this->cts = pts - dts;

	::memcpy(&video_tag[1], data, sz);
	return sz;
}

ssize_t FLVVideoTag::writeData(MediaContext* ctx, uint8_t* data, size_t sz, uint32_t dts, uint32_t pts) {
	if(!data || !ctx || !sz)
	{
		::perror("null pointer argument");
		::exit(-1);
	}

	FLVTag* tag = (FLVTag*) ctx;
	flv_video_t* video_tag = (flv_video_t*) obj_buffer;
	if(codec_id == H264_AVC)
	{
		tag->setSize(sz + sizeof(video_tag));
		// TODO: We Improve here -> we can check bit pattern to decide whether NALU or not
		this->avc_type = AVC_NALU;
	}
	else
	{
		tag->setSize(sz + sizeof(video_tag->flags));
	}
	tag->setTimestamp(dts);
	tag->setType(FLVTag::Video);

	this->cts = pts - dts;

	::memcpy(&video_tag[1], data, sz);
	return sz;
}

ssize_t FLVVideoTag::readData(const MediaContext* ctx, uint8_t* data) {
	if(!data || !ctx)
	{
		::perror("null pointer argument");
		::exit(-1);
	}

	size_t psz = 0, hsz = 0;
	const FLVTag* tag = (const FLVTag*) ctx;
	psz = tag->getSize();
	flv_video_t* video_tag = (flv_video_t*) obj_buffer;
	if(this->codec_id)
	{
		hsz = sizeof(flv_video_t);
	}
	else
	{
		hsz = sizeof(video_tag->flags);
	}

	::memcpy(data, &video_tag[1], psz - hsz);
	return psz - hsz;
}

FLVVideoTag::AVCPktType FLVVideoTag::getAvcType() const{
	return avc_type;
}

void FLVVideoTag::setAvcType(AVCPktType avcType) {
	avc_type = avcType;
}

FLVVideoTag::CodecID FLVVideoTag::getCodecId() const {
	return codec_id;
}

void FLVVideoTag::setCodecId(CodecID codecId) {
	codec_id = codecId;
}

FLVVideoTag::FrameType FLVVideoTag::getFrameType() const {
	return frame_type;
}

void FLVVideoTag::setFrameType(FrameType frameType) {
	frame_type = frameType;
}



FLVDataScriptTag::FLVDataScriptTag() {
	amf_script = new AMF0();
}

FLVDataScriptTag::~FLVDataScriptTag() {

}


ssize_t FLVDataScriptTag::serialize(MediaContext* ctx,
		MediaStream* stream) {
	if(!stream)
	{
		return -1;
	}
	return amf_script->serialize(ctx, stream);
}

ssize_t FLVDataScriptTag::deserialize(MediaContext* ctx, MediaStream* stream) {
	if(!stream)
	{
		return -1;
	}
	return amf_script->deserialize(ctx, stream);
}

ssize_t FLVDataScriptTag::readData(const MediaContext* ctx, uint8_t* data) {
	if(!data)
	{
		return -1;
	}
	return amf_script->read(data);
}

MediaPipe::FLVPayload::FLVPayload() { }

MediaPipe::FLVPayload::~FLVPayload() { }


}

static uint32_t __bswap_u24_to_u32(uint24_t* u)
{
	return (u->_[0] << 16) | (u->_[1] << 8) | (u->_[2]);
}

static void __bswap_u32_to_u24(uint32_t s, uint24_t* u)
{
	s &= 0xFFFFFF;
	u->_[0] = s >> 16;
	u->_[1] = s >> 8;
	u->_[2] = s & 0xFF;
}


