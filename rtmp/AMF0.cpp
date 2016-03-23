/*
 * AMF0.cpp
 *
 *  Created on: Mar 19, 2016
 *      Author: innocentevil
 */


#include "cdsl_slist.h"
#include "mpipe.h"
#include "AMF0.h"
#include "FLVTag.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint8_t _[3];
}__attribute__((packed)) uint24_t;

struct amf0_str {
	uint16_t 		len;
	char			str_seq;
} __attribute__((packed));

struct amf0_obj_prop {
	amf0_str 		name;
	uint8_t			amf0_v;
} __attribute__((packed));

struct amf0_obj_end {
	uint24_t		end_marker;
}__attribute__((packed));

struct amf0_obj {
	amf0_obj_prop	props;		// array of amf0_obj_prop
	// obj_end marker will follow at the end of array
}__attribute__((packed));

struct amf0_ecma_array {
	uint32_t 		len;
	amf0_obj_prop	props;		// array of amf0_obj_prop
	// obj_end marker will follow at the end of array
}__attribute__((packed));

struct amf0_strict_array {
	uint32_t		len;
	amf0_obj_prop	props;
}__attribute__((packed));

struct amf0_date {
	double 	epoch_mills;
	int16_t	utc_adj;
}__attribute__((packed));


struct amf0_lstr {
	uint32_t		len;
	char			str_seq;
}__attribute__((packed));

struct amf0_val {
	uint8_t marker;
	union {
		double numv;
		uint8_t boolv;
		struct amf0_str strv;
	} __attribute__((packed));
} __attribute__((packed));

namespace MediaPipe {

AMF0Base::AMF0Base()
{
	cdsl_slistNodeInit(this);
}

AMF0Base::~AMF0Base()
{

}


AMF0::AMF0Iterator::AMF0Iterator(AMF0* iterable,bool is_mutable) {
}

AMF0::AMF0Iterator::~AMF0Iterator() {
}

bool AMF0::AMF0Iterator::hasNext() {
}

AMF0Base* AMF0::AMF0Iterator::next() {
}

void AMF0::AMF0Iterator::remove() {
}

AMF0::AMF0(bool is_mutable)
{
	cdsl_slistEntryInit(this);
}

AMF0::AMF0()
{
	is_mutable = true;
}

AMF0::~AMF0()
{
	while(!cdsl_slistIsEmpty(this))
	{
	}
}
ssize_t AMF0::serialize(void* ctx, uint8_t* into) {

}

ssize_t AMF0::serialize(void* ctx, MediaStream* stream)
{

}

ssize_t AMF0::deserialize(void* ctx, const uint8_t* from)
{

}


ssize_t AMF0::deserialize(void* ctx, const MediaStream* stream)
{
	if(!stream)
		return -1;
	AMF0Base* val = NULL;
	AMF0Base::AMF0Type type;
	FLVTag* flv_t = (FLVTag*) ctx;
	size_t t_sz = flv_t->getSize();
	size_t sz = 0;
	while(sz < t_sz)
	{
		if(stream->read((uint8_t*) &type, sizeof(uint8_t)) < 0)
			return sz;
		sz++;
		switch(type){
		case AMF0Base::NUMBER:
			val = (AMF0Base*)  new AMF0NumberData();
			sz += val->deserialize(ctx, stream);
			break;
		case AMF0Base::BOOLEAN:
			val = (AMF0Base*) new AMF0BooleanData();
			sz += val->deserialize(ctx, stream);
			break;
		case AMF0Base::STRING:
			val = (AMF0Base*) new AMF0StringData();
			sz += val->deserialize(ctx, stream);
			break;
		case AMF0Base::OBJECT:
			val = (AMF0Base*) new AMF0ObjectData();
			sz += val->deserialize(ctx, stream);
			break;
		case AMF0Base::REF:
			val = (AMF0Base*) new AMF0ReferenceData();
			sz += val->deserialize(ctx, stream);
			break;
		case AMF0Base::ECMA_ARRAY:
			val = (AMF0Base*) new AMF0ECMAArrayData();
			sz += val->deserialize(ctx, stream);
			break;
		case AMF0Base::OBJ_END:
			// object end is not handled here
			break;
		case AMF0Base::UND:
			break;
		case AMF0Base::NUL:
			break;
		case AMF0Base::MV_CLIP:
			break;
		case AMF0Base::STRICT_ARRAY:
			val = (AMF0Base*) new AMF0StrictArrayData();
			sz += val->deserialize(ctx, stream);
			break;
		case AMF0Base::DATE:
			val = (AMF0Base*) new AMF0DateData();
			sz += val->deserialize(ctx, stream);
			break;
		case AMF0Base::LSTRING:
			val = (AMF0Base*) new AMF0LongStringData();
			sz += val->deserialize(ctx, stream);
			break;
		}
		cdsl_slistPutTail(this,val);
	}
	return sz;
}


Iterator<AMF0Base>* AMF0::iterator() {
	return new AMF0Iterator(this,is_mutable);
}


int AMF0::add(AMF0Base* obj)
{
	return cdsl_slistPutTail(this,obj);
}

void AMF0::remove(AMF0Base* obj)
{
	cdsl_slistRemove(this, obj);
}

void AMF0::setMutable(bool is_mutable)
{

}



int AMF0::length(void)
{
	return cdsl_slistSize(this);
}


AMF0NumberData::AMF0NumberData()
{

}

AMF0NumberData::~AMF0NumberData()
{

}

ssize_t AMF0NumberData::serialize(void* ctx, MediaStream* stream)
{
}

ssize_t AMF0NumberData::serialize(void* ctx, uint8_t* into) {
}

ssize_t AMF0NumberData::deserialize(void* ctx, const MediaStream* stream) {
}


ssize_t AMF0NumberData::deserialize(void* ctx, const uint8_t* from) {
}

AMF0Base::AMF0Type AMF0NumberData::getType()
{

}

double AMF0NumberData::getValue()
{

}

AMF0BooleanData::AMF0BooleanData()
{

}

AMF0BooleanData::~AMF0BooleanData() {
}

ssize_t AMF0BooleanData::serialize(void* ctx, MediaStream* stream) {
}

ssize_t AMF0BooleanData::serialize(void* ctx, uint8_t* into) {
}

ssize_t AMF0BooleanData::deserialize(void* ctx, const MediaStream* stream) {
}

ssize_t AMF0BooleanData::deserialize(void* ctx, const uint8_t* from) {
}

AMF0Base::AMF0Type AMF0BooleanData::getType()
{

}

bool AMF0BooleanData::getValue()
{

}


AMF0StringData::AMF0StringData() {
}

AMF0StringData::~AMF0StringData() {
}

ssize_t AMF0StringData::serialize(void* ctx, MediaStream* stream) {
}

ssize_t AMF0StringData::serialize(void* ctx, uint8_t* into) {
}

ssize_t AMF0StringData::deserialize(void* ctx, const MediaStream* stream) {
}

ssize_t AMF0StringData::deserialize(void* ctx, const uint8_t* from) {
}

AMF0Base::AMF0Type AMF0StringData::getType()
{

}

const std::string* AMF0StringData::getValue()
{

}

AMF0ObjectData::AMF0ObjectData() {
}

AMF0ObjectData::~AMF0ObjectData() {
}

ssize_t AMF0ObjectData::serialize(void* ctx, MediaStream* stream) {
}

ssize_t AMF0ObjectData::serialize(void* ctx, uint8_t* into) {
}

ssize_t AMF0ObjectData::deserialize(void* ctx, const MediaStream* stream) {
}

ssize_t AMF0ObjectData::deserialize(void* ctx, const uint8_t* from) {
}

AMF0Base::AMF0Type AMF0ObjectData::getType()
{

}

const AMF0* AMF0ObjectData::getValue()
{

}

AMF0ReferenceData::AMF0ReferenceData() {

}

AMF0ReferenceData::~AMF0ReferenceData() {

}


ssize_t AMF0ReferenceData::serialize(void* ctx, MediaStream* stream) {

}

ssize_t AMF0ReferenceData::serialize(void* ctx, uint8_t* into) {

}

ssize_t AMF0ReferenceData::deserialize(void* ctx, const MediaStream* stream) {

}

ssize_t AMF0ReferenceData::deserialize(void* ctx, const uint8_t* from) {

}

AMF0Base::AMF0Type AMF0ReferenceData::getType()
{

}

uint16_t AMF0ReferenceData::getValue()
{

}

AMF0ECMAArrayData::AMF0ECMAArrayData() {
}

AMF0ECMAArrayData::~AMF0ECMAArrayData() {
}

ssize_t AMF0ECMAArrayData::serialize(void* ctx, MediaStream* stream) {
}

ssize_t AMF0ECMAArrayData::serialize(void* ctx, uint8_t* into) {
}

ssize_t AMF0ECMAArrayData::deserialize(void* ctx, const MediaStream* stream) {
}

ssize_t AMF0ECMAArrayData::deserialize(void* ctx, const uint8_t* from) {
}

AMF0Base::AMF0Type AMF0ECMAArrayData::getType()
{

}

const AMF0* AMF0ECMAArrayData::getValue()
{

}


AMF0StrictArrayData::AMF0StrictArrayData() {
}

AMF0StrictArrayData::~AMF0StrictArrayData() {
}

ssize_t AMF0StrictArrayData::serialize(void* ctx, MediaStream* stream) {
}

ssize_t AMF0StrictArrayData::serialize(void* ctx, uint8_t* into) {
}

ssize_t AMF0StrictArrayData::deserialize(void* ctx, const MediaStream* stream) {
}

ssize_t AMF0StrictArrayData::deserialize(void* ctx, const uint8_t* from) {
}

AMF0Base::AMF0Type AMF0StrictArrayData::getType()
{

}

const AMF0* AMF0StrictArrayData::getValue()
{

}

AMF0DateData::AMF0DateData() {
}

AMF0DateData::~AMF0DateData() {
}

ssize_t AMF0DateData::serialize(void* ctx, MediaStream* stream) {
}

ssize_t AMF0DateData::serialize(void* ctx, uint8_t* into) {
}

ssize_t AMF0DateData::deserialize(void* ctx, const MediaStream* stream) {
}

ssize_t AMF0DateData::deserialize(void* ctx, const uint8_t* from) {
}

AMF0Base::AMF0Type AMF0DateData::getType()
{

}

time_t AMF0DateData::getValue()
{

}

AMF0LongStringData::AMF0LongStringData() {
}

AMF0LongStringData::~AMF0LongStringData() {
}

ssize_t AMF0LongStringData::serialize(void* ctx, MediaStream* stream) {
}

ssize_t AMF0LongStringData::serialize(void* ctx, uint8_t* into) {
}

ssize_t AMF0LongStringData::deserialize(void* ctx, const MediaStream* stream) {
}

ssize_t AMF0LongStringData::deserialize(void* ctx, const uint8_t* from) {
}

AMF0Base::AMF0Type AMF0LongStringData::getType()
{

}

const std::string* AMF0LongStringData::getValue()
{

}

} /* namespace MediaPipe */
