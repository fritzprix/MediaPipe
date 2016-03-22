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

AMF0::AMF0Base::AMF0Base()
{
	cdsl_slistNodeInit(this);
}

AMF0::AMF0Base::~AMF0Base()
{

}


AMF0::Iterator::Iterator(AMF0* amf)
{
	cdsl_slistIterInit(&amf->mutable_lentry,this);
}

AMF0::Iterator::~Iterator(){ }

bool AMF0::Iterator::hasNext()
{
	return cdsl_slistIterHasNext(this);
}

AMF0::AMF0Base* AMF0::Iterator::getNext()
{
	return (AMF0::AMF0Base*) cdsl_slistIterNext(this);
}

void AMF0::Iterator::remove()
{
	cdsl_slistIterRemove(this);
}

AMF0::AMF0()
{
	cdsl_slistEntryInit(&immutable_lentry);
	cdsl_slistEntryInit(&mutable_lentry);
	iter = new Iterator(this);
}

AMF0::~AMF0()
{
	while(!cdsl_slistIsEmpty(&immutable_lentry))
	{
	}
}

ssize_t AMF0::serialize(MediaContext* ctx, MediaStream* stream)
{

}

ssize_t AMF0::deserialize(MediaContext* ctx, MediaStream* stream)
{
	if(!stream)
		return -1;
	AMF0::AMF0Base* val = NULL;
	AMF0::AMF0Type type;
	FLVTag* flv_t = (FLVTag*) ctx;
	size_t t_sz = flv_t->getSize();
	size_t sz = 0;
	while(sz < t_sz)
	{
		if(stream->read((uint8_t*) &type, sizeof(uint8_t)) < 0)
			return sz;
		sz++;
		switch(type){
		case NUMBER:
			val = (AMF0::AMF0Base*)  new AMF0NumberData();
			sz += val->deserialize(ctx, stream);
			break;
		case BOOLEAN:
			val = (AMF0::AMF0Base*) new AMF0BooleanData();
			sz += val->deserialize(ctx, stream);
			break;
		case STRING:
			val = (AMF0::AMF0Base*) new AMF0StringData();
			sz += val->deserialize(ctx, stream);
			break;
		case OBJECT:
			val = (AMF0::AMF0Base*) new AMF0ObjectData();
			sz += val->deserialize(ctx, stream);
			break;
		case REF:
			val = (AMF0::AMF0Base*) new AMF0ReferenceData();
			sz += val->deserialize(ctx, stream);
			break;
		case ECMA_ARRAY:
			val = (AMF0::AMF0Base*) new AMF0ECMAArrayData();
			sz += val->deserialize(ctx, stream);
			break;
		case OBJ_END:
			// object end is not handled here
			break;
		case UND:
			break;
		case NUL:
			break;
		case MV_CLIP:
			break;
		case STRICT_ARRAY:
			val = (AMF0::AMF0Base*) new AMF0StrictArrayData();
			sz += val->deserialize(ctx, stream);
			break;
		case DATE:
			val = (AMF0::AMF0Base*) new AMF0DateData();
			sz += val->deserialize(ctx, stream);
			break;
		case LSTRING:
			val = (AMF0::AMF0Base*) new AMF0LongStringData();
			sz += val->deserialize(ctx, stream);
			break;
		}
		cdsl_slistPutTail(&immutable_lentry,val);
	}
	return sz;
}

ssize_t AMF0::read(uint8_t* data)
{

}



int AMF0::add(AMF0Base* obj)
{
	return cdsl_slistPutTail(&mutable_lentry,obj);
}

void AMF0::remove(AMF0Base* obj)
{
	cdsl_slistRemove(&mutable_lentry, obj);
}

int AMF0::length(void)
{
	return cdsl_slistSize(&mutable_lentry) + cdsl_slistSize(&immutable_lentry);
}

AMF0NumberData::AMF0NumberData() {
	val = 0.0;
}

AMF0NumberData::~AMF0NumberData() {
}

ssize_t AMF0NumberData::serialize(MediaContext* ctx,
		MediaStream* stream) {
}

ssize_t AMF0NumberData::deserialize(MediaContext* ctx,
		MediaStream* stream) {
}

double AMF0NumberData::getValue() {
}

AMF0::AMF0Type AMF0NumberData::getType() {
}

AMF0BooleanData::AMF0BooleanData() {
	val = false;
}

AMF0BooleanData::~AMF0BooleanData() {

}

ssize_t AMF0BooleanData::serialize(MediaContext* ctx,
		MediaStream* stream) {
	::printf("bool serialized\n");
}

ssize_t AMF0BooleanData::deserialize(MediaContext* ctx,
		MediaStream* stream) {
}

AMF0::AMF0Type AMF0BooleanData::getType() {
}

bool AMF0BooleanData::getValue() {
}

AMF0StringData::AMF0StringData() {
}

AMF0StringData::~AMF0StringData() {
}

ssize_t AMF0StringData::serialize(MediaContext* ctx,
		MediaStream* stream) {
}

ssize_t AMF0StringData::deserialize(MediaContext* ctx,
		MediaStream* stream) {
}

AMF0::AMF0Type AMF0StringData::getType() {
}

const std::string* AMF0StringData::getValue() {
}

AMF0ObjectData::AMF0ObjectData() {
}

AMF0ObjectData::~AMF0ObjectData() {
}

ssize_t AMF0ObjectData::serialize(MediaContext* ctx,
		MediaStream* stream) {
}

ssize_t AMF0ObjectData::deserialize(MediaContext* ctx,
		MediaStream* stream) {
}

AMF0::AMF0Type AMF0ObjectData::getType() {
}

const AMF0* AMF0ObjectData::getValue() {
}

AMF0ReferenceData::AMF0ReferenceData() {
}

AMF0ReferenceData::~AMF0ReferenceData() {
}

ssize_t AMF0ReferenceData::serialize(MediaContext* ctx,
		MediaStream* stream) {
}

ssize_t AMF0ReferenceData::deserialize(MediaContext* ctx,
		MediaStream* stream) {
}

AMF0::AMF0Type AMF0ReferenceData::getType() {
}

uint16_t AMF0ReferenceData::getValue() {
}

AMF0ECMAArrayData::AMF0ECMAArrayData() {
}

AMF0ECMAArrayData::~AMF0ECMAArrayData() {
}

ssize_t AMF0ECMAArrayData::serialize(MediaContext* ctx,
		MediaStream* stream) {
}

ssize_t AMF0ECMAArrayData::deserialize(MediaContext* ctx,
		MediaStream* stream) {
}

AMF0::AMF0Type AMF0ECMAArrayData::getType() {
}

const AMF0* AMF0ECMAArrayData::getValue() {
}

AMF0StrictArrayData::AMF0StrictArrayData() {
}

AMF0StrictArrayData::~AMF0StrictArrayData() {
}

ssize_t AMF0StrictArrayData::serialize(MediaContext* ctx,
		MediaStream* stream) {
}

ssize_t AMF0StrictArrayData::deserialize(MediaContext* ctx,
		MediaStream* stream) {
}

AMF0::AMF0Type AMF0StrictArrayData::getType() {
}

const AMF0* AMF0StrictArrayData::getValue() {
}

AMF0DateData::AMF0DateData() {
}

AMF0DateData::~AMF0DateData() {
}

ssize_t AMF0DateData::serialize(MediaContext* ctx,
		MediaStream* stream) {
}

ssize_t AMF0DateData::deserialize(MediaContext* ctx,
		MediaStream* stream) {
}

AMF0::AMF0Type AMF0DateData::getType() {
}

time_t AMF0DateData::getValue() {
}

AMF0LongStringData::AMF0LongStringData() {
}

AMF0LongStringData::~AMF0LongStringData() {
}

ssize_t AMF0LongStringData::serialize(MediaContext* ctx,
		MediaStream* stream) {
}

ssize_t AMF0LongStringData::deserialize(MediaContext* ctx,
		MediaStream* stream) {
}

AMF0::AMF0Type AMF0LongStringData::getType() {
}

const std::string* AMF0LongStringData::getValue() {
}

} /* namespace MediaPipe */
