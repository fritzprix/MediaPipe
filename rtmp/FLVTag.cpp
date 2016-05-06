/*
 * FLVTag.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: innocentevil
 */

#include <FLVTag.h>
#include "Iterator.h"


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

FLVTag::FLVTag() { }
FLVTag::~FLVTag(){ }

FLVCommonTag::FLVCommonTag(FLVCommonTag* tag) {
	type = tag->type;
	t_stmp = tag->t_stmp;
	psize = tag->psize;
	stream_id = tag->stream_id;
}

FLVCommonTag::FLVCommonTag() {
	type = Video;
	t_stmp = 0;
	psize = 0;
	stream_id = 0;
}

FLVCommonTag::~FLVCommonTag() { }



ssize_t FLVCommonTag::serialize(void* ctx,MediaStream* stream) {
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

ssize_t FLVCommonTag::deserialize(void* ctx, const MediaStream* stream) {
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

ssize_t FLVCommonTag::serialize(void* ctx, uint8_t* into) {
	flv_tag_t tag;
	if(!into)
	{
		return -1;
	}
	tag.type = type;
	tag.time_stmp_ex = t_stmp >> 24;
	__bswap_u32_to_u24(this->psize, &tag.data_sz);
	__bswap_u32_to_u24(this->t_stmp, &tag.time_stmp);
	__bswap_u32_to_u24(this->stream_id, &tag.stream_id);
	memcpy(into, &tag, sizeof(flv_tag_t));
	return sizeof(flv_tag_t);
}

ssize_t FLVCommonTag::deserialize(void* ctx, const uint8_t* from) {
	flv_tag_t tag;
	if(!from)
	{
		return -1;
	}
	memcpy(&tag, from, sizeof(flv_tag_t));
	type = (TagType) tag.type;
	t_stmp = (tag.time_stmp_ex << 24) | __bswap_u24_to_u32(&tag.time_stmp);
	psize = __bswap_u24_to_u32(&tag.data_sz);
	stream_id = __bswap_u24_to_u32(&tag.stream_id);
	return sizeof(flv_tag_t);
}


FLVCommonTag::TagType FLVCommonTag::getType(void) const{
	return type;
}

void FLVCommonTag::setType(TagType type) {
	this->type = type;
}

size_t FLVCommonTag::getSize(void) const {
	return psize;
}

void FLVCommonTag::setSize(size_t size) {
	this->psize = size;
}

uint32_t FLVCommonTag::getTimestamp(void) const {
	return t_stmp;
}

void FLVCommonTag::setTimestamp(uint32_t timestamp) {
	this->t_stmp = timestamp;
}

uint32_t FLVCommonTag::getStreamID(void) const {
	return stream_id;
}

void FLVCommonTag::setStreamID(uint32_t stream_id) {
	this->stream_id = stream_id;
}

FLVAudioTag::FLVAudioTag() {

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
}

FLVAudioTag::~FLVAudioTag() {
}


ssize_t FLVAudioTag::serialize(FLVCommonTag* ctx,MediaStream* stream) {
	if(!stream || !ctx)
	{
		::perror("null pointer argument !!");
		::exit(-1);
	}
	ssize_t res;
	size_t rsz = 0;
	flv_audio_t atag;
	atag.flags = (this->snd_format << 4) | \
						(this->snd_rate << 2) | \
						(this->snd_sz << 1) | \
						(this->snd_type);
	if((res = stream->write(&atag, sizeof(atag.flags))) < 0)
		return res;
	rsz += res;

	if(snd_format != AAC)
		return rsz;

	// if audio tag is AAC type
	// aac pkt type should be written into stream
	atag.aac_ex.aac_pkt_type = this->aac_pkt_type;
	if((res = stream->write(&atag.aac_ex,sizeof(flv_aac_audio_t))) < 0)
		return res;
	rsz += res;
	return rsz;
}

ssize_t FLVAudioTag::deserialize(FLVCommonTag* ctx, const MediaStream* stream) {
	if(!stream || !ctx)
	{
		::perror("null pointer argument !!");
		::exit(-1);
	}

	ssize_t res = 0;
	size_t rsz = 0;
	flv_audio_t atag;
	commonTag = *ctx;
	if((res = stream->read(&atag.flags,sizeof(atag.flags))) < 0)
	{
		return res;
	}
	rsz += res;
	snd_format = (SoundFormat) (atag.flags >> 4);
	snd_rate = (SoundRate) ((atag.flags >> 2) & 3);
	snd_sz = (SoundSize) ((atag.flags >> 1) & 1);
	snd_type = (SoundType) (atag.flags & 1);
	if(snd_format != AAC)
	{
		commonTag.setSize(commonTag.getSize() - rsz);
		return rsz;
	}
	if((res = stream->read(&atag.aac_ex, sizeof(flv_aac_audio_t))) < 0)
		return res;
	rsz += res;
	aac_pkt_type = (AACPktType) atag.aac_ex.aac_pkt_type;
	commonTag.setSize(commonTag.getSize() - rsz);
	return rsz;
}



ssize_t FLVAudioTag::serialize(FLVCommonTag* ctx, uint8_t* into) {
	if(!into || !ctx) {
		::perror("null pointer argument !!");
		::exit(-1);
	}
	flv_audio_t atag;
	/*
	 * get payload size in bytes
	 * payload size = size of audio_tag + size of raw audio data in bytes
	 */

	// calculate header size depends on sound format
	atag.flags = (snd_format << 4) | \
			     (snd_rate << 2) | \
				 (snd_sz << 1) | \
				 snd_type;

	if(snd_format != AAC)
	{
		memcpy(into, &atag, sizeof(atag.flags));
		return sizeof(atag.flags);
	}
	atag.aac_ex.aac_pkt_type = aac_pkt_type;
	memcpy(into, &atag, sizeof(flv_audio_t));
	return sizeof(flv_audio_t);
}


ssize_t FLVAudioTag::deserialize(FLVCommonTag* ctx, const uint8_t* from) {
	if(!ctx || !from) {
		::perror("null pointer argument !!");
		::exit(-1);
	}

	flv_audio_t atag;
	commonTag = *ctx;

	memcpy(&atag, from, sizeof(atag.flags));
	snd_format = (SoundFormat) (atag.flags >> 4);
	snd_rate = (SoundRate) ((atag.flags >> 2) & 3);
	snd_sz = (SoundSize) ((atag.flags >> 1) & 1);
	snd_type = (SoundType) (atag.flags & 1);

	if(snd_format != AAC)
	{
		commonTag.setSize(commonTag.getSize() - sizeof(atag.flags));
		return sizeof(atag.flags);
	}
	memcpy(&atag.aac_ex, from, sizeof(flv_aac_audio_t));
	aac_pkt_type = (AACPktType) atag.aac_ex.aac_pkt_type;
	commonTag.setSize(commonTag.getSize() - sizeof(flv_audio_t));
	return sizeof(flv_audio_t);
}

FLVTag::TagType FLVAudioTag::getType(void) const
{
	return commonTag.getType();
}

void FLVAudioTag::setType(FLVTag::TagType)
{

}

size_t FLVAudioTag::getSize(void) const
{
	return commonTag.getSize();
}

void FLVAudioTag::setSize(size_t)
{

}

uint32_t FLVAudioTag::getTimestamp(void) const
{
	return commonTag.getTimestamp();
}

void FLVAudioTag::setTimestamp(uint32_t)
{

}

uint32_t FLVAudioTag::getStreamID(void) const
{
	return commonTag.getStreamID();
}

void FLVAudioTag::setStreamID(uint32_t)
{

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


FLVAudioTag::SoundType FLVAudioTag::getSoundType() const {
	return snd_type;
}


FLVVideoTag::FLVVideoTag() {
	frame_type = KEYFRAME;
	codec_id = H264_AVC;
	avc_type = AVC_SeqHeader;
	cts = 0;
}

FLVVideoTag::~FLVVideoTag() {
}

ssize_t FLVVideoTag::serialize(FLVCommonTag* ctx, MediaStream* stream) {
	if(!stream || !ctx)
	{
		::perror("null pointer argument !\n");
		::exit(-1);
	}

	flv_video_t vtag;
	vtag.flags = ((frame_type << 4) | (codec_id & 0xFF));
	if(codec_id != H264_AVC)
	{
		return stream->write(&vtag,sizeof(vtag.flags));
	}
	// if h.264 chunk fill out AVC extended tag
	__bswap_u32_to_u24(cts, &vtag.avc_ex.cps_time);// cts is defined (pts - dts)
	vtag.avc_ex.pkt_type = avc_type;
	return stream->write(&vtag, sizeof(flv_video_t));
}

ssize_t FLVVideoTag::serialize(FLVCommonTag* ctx, uint8_t* into) {
	if(!into || !ctx)
	{
		::perror("null pointer argument");
		::exit(-1);
	}

	flv_video_t vtag;
	vtag.flags = ((frame_type << 4) | (codec_id & 0xF));
	if(this->codec_id != FLVVideoTag::H264_AVC)
	{
		memcpy(into, &vtag,sizeof(vtag.flags));
		return sizeof(vtag.flags);
	}
	vtag.avc_ex.pkt_type = avc_type;
	__bswap_u32_to_u24(cts, &vtag.avc_ex.cps_time);
	memcpy(into, &vtag, sizeof(flv_video_t));
	return sizeof(flv_video_t);
}

ssize_t FLVVideoTag::deserialize(FLVCommonTag* ctx, const MediaStream* stream) {
	if(!stream || !ctx)
	{
		::perror("null pointer argument !\n");
		::exit(-1);
	}
	commonTag = *ctx;

	ssize_t res;
	size_t rsz = 0;
	flv_video_t vtag;
	if((res = stream->read(&vtag.flags, sizeof(vtag.flags))) < 0)
		return res;
	rsz += res;
	frame_type = (FrameType) (vtag.flags >> 4);
	codec_id = (CodecID) (vtag.flags & 0xF);

	if(codec_id == H264_AVC)
	{
		if((res = stream->read(&vtag.avc_ex,sizeof(flv_video_avc_t))) < 0)
			return res;
		rsz += res;
		// if codec is consistent
		// that means this chunk is H264
		// so fill avc packet header part
		cts = __bswap_u24_to_u32(&vtag.avc_ex.cps_time);
		avc_type = (AVCPktType) vtag.avc_ex.pkt_type;
	}
	commonTag.setSize(commonTag.getSize() - rsz);
	return rsz;
}



ssize_t FLVVideoTag::deserialize(FLVCommonTag* ctx, const uint8_t* from) {
	if(!ctx || !from)
	{
		::perror("null pointer argument !\n");
		::exit(-1);
	}
	commonTag = *ctx;

	size_t rsz = 0;
	flv_video_t vtag;
	memcpy(&vtag.flags,from,sizeof(vtag.flags));
	rsz += sizeof(vtag.flags);
	frame_type = (FrameType) (vtag.flags >> 4);
	codec_id = (CodecID) (vtag.flags & 0xF);

	if(codec_id == H264_AVC)
	{
		memcpy(&vtag.avc_ex, from, sizeof(flv_video_avc_t));
		rsz += sizeof(flv_video_avc_t);
		// if codec is consistent
		// that means this chunk is H264
		// so fill avc packet header part
		cts = __bswap_u24_to_u32(&vtag.avc_ex.cps_time);
		avc_type = (AVCPktType) vtag.avc_ex.pkt_type;
	}
	commonTag.setSize(commonTag.getSize() - rsz);
	return rsz;
}


FLVTag::TagType FLVVideoTag::getType(void) const
{
	return commonTag.getType();
}

void FLVVideoTag::setType(FLVTag::TagType)
{

}

size_t FLVVideoTag::getSize(void) const
{
	return commonTag.getSize();
}

void FLVVideoTag::setSize(size_t)
{

}

uint32_t FLVVideoTag::getTimestamp(void) const
{
	return commonTag.getTimestamp();
}

void FLVVideoTag::setTimestamp(uint32_t)
{

}

uint32_t FLVVideoTag::getStreamID(void) const
{
	return commonTag.getStreamID();
}

void FLVVideoTag::setStreamID(uint32_t)
{

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





FLVDataScriptTag::FLVDataScriptTag() { }

FLVDataScriptTag::~FLVDataScriptTag() { }


ssize_t FLVDataScriptTag::serialize(FLVCommonTag* ctx, MediaStream* stream) {
	if(!ctx || !stream)
		return -1;
	return 0;
}

ssize_t FLVDataScriptTag::serialize(FLVCommonTag* ctx, uint8_t* data) {
	if(!ctx || !data)
		return -1;
	return 0;
}

ssize_t FLVDataScriptTag::deserialize(FLVCommonTag* ctx, const  MediaStream* stream) {
	if(!ctx || !stream)
		return -1;
	commonTag = *ctx;
	return 0;
}


ssize_t FLVDataScriptTag::deserialize(FLVCommonTag* ctx, const uint8_t* from) {
	if(!ctx || !from)
		return -1;
	commonTag = *ctx;
	return 0;
}

FLVTag::TagType FLVDataScriptTag::getType(void) const
{
	return commonTag.getType();
}

void FLVDataScriptTag::setType(FLVTag::TagType)
{
}

size_t FLVDataScriptTag::getSize(void) const
{
	return commonTag.getSize();
}

void FLVDataScriptTag::setSize(size_t)
{
}

uint32_t FLVDataScriptTag::getTimestamp(void) const
{
	return commonTag.getTimestamp();
}

void FLVDataScriptTag::setTimestamp(uint32_t)
{

}

uint32_t FLVDataScriptTag::getStreamID(void) const
{
	return commonTag.getStreamID();
}

void FLVDataScriptTag::setStreamID(uint32_t)
{

}


}		// end of namespace MediaPipe


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





