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
#include <string.h>
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
	cdsl_slistIterInit(iterable,this);
	this->is_mutable = is_mutable;
}

AMF0::AMF0Iterator::~AMF0Iterator() {
	/*
	 * nothing to be cleaned
	 */
}

bool AMF0::AMF0Iterator::hasNext() {
	return cdsl_iterHasNext(this);
}

AMF0Base* AMF0::AMF0Iterator::next() {
	return (AMF0Base*) cdsl_iterNext(this);
}

void AMF0::AMF0Iterator::remove() {
	if(!is_mutable)
		return;
	cdsl_iterRemove(this);
}

AMF0::AMF0(bool is_mutable)
{
	/*
	 * mutable means AMF0Base object can be added or removed by add / remove method
	 * in this case AMF0Base should be freed outside of AMF0.
	 */
	cdsl_slistEntryInit(this);
	this->is_mutable = is_mutable;
}

AMF0::AMF0()
{
	cdsl_slistEntryInit(this);
	is_mutable = true;
}

AMF0::~AMF0()
{
	while(!cdsl_slistIsEmpty(this))
	{
		AMF0Base* amf0 = (AMF0Base*) cdsl_slistDequeue(this);
		if(!is_mutable)
		{
			delete amf0;
		}
	}
}

ssize_t AMF0::serialize(void* ctx, uint8_t* into) {
	if(!into)
		return -1;

	listIter_t iter;
	cdsl_iterInit((listEntry_t*) this, &iter);
	AMF0Base* amf0;
	size_t sz = 0;
	while(cdsl_iterHasNext(&iter)) {
		amf0 = (AMF0Base*) cdsl_iterNext(&iter);
		sz += amf0->serialize(ctx, into);
	}
	return sz;
}

ssize_t AMF0::serialize(void* ctx, MediaStream* stream)
{
	if(!stream)
		return -1;

	listIter_t iter;
	cdsl_iterInit((listEntry_t*) this, &iter);
	AMF0Base* amf0;
	size_t sz = 0;
	while(cdsl_iterHasNext(&iter)) {
		amf0 = (AMF0Base*) cdsl_iterNext(&iter);
		sz += amf0->serialize(ctx, stream);
	}
	return sz;
}

ssize_t AMF0::deserialize(void* ctx, const uint8_t* from)
{
	if(!from)
		return -1;

	FLVTag* flv_t = (FLVTag*) ctx;
	AMF0Base::AMF0Type type;
	AMF0Base *val = NULL;
	size_t t_sz = flv_t->getSize();
	size_t sz = 0;
	uint8_t* cptr = (uint8_t*)from;
	while(sz <= t_sz)
	{
		type = (AMF0Base::AMF0Type) *cptr;
		sz++;
		val = NULL;
		switch(type) {
		case AMF0Base::NUMBER:
			val = new AMF0NumberData();
			break;
		case AMF0Base::BOOLEAN:
			val = new AMF0BooleanData();
			break;
		case AMF0Base::STRING:
			val = new AMF0StringData();
			break;
		case AMF0Base::OBJECT:
			val = new AMF0ObjectData();
			break;
		case AMF0Base::REF:
			val = new AMF0ReferenceData();
			break;
		case AMF0Base::ECMA_ARRAY:
			val = new AMF0ECMAArrayData();
			break;
		case AMF0Base::OBJ_END:
			break;
		case AMF0Base::UND:
			break;
		case AMF0Base::NUL:
			break;
		case AMF0Base::MV_CLIP:
			break;
		case AMF0Base::STRICT_ARRAY:
			val = new AMF0StrictArrayData();
			break;
		case AMF0Base::DATE:
			val = new AMF0DateData();
			break;
		case AMF0Base::LSTRING:
			val = new AMF0LongStringData();
			break;
		}
		if(val) {
			sz += val->deserialize(ctx, from);
			cdsl_slistPutHead(this, val);
		}
	}
	return sz;
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
	while(sz <= t_sz)
	{
		if(stream->read((uint8_t*) &type, sizeof(uint8_t)) < 0)
			return sz;
		sz++;
		val = NULL;
		switch(type){
		case AMF0Base::NUMBER:
			val = (AMF0Base*)  new AMF0NumberData();
			break;
		case AMF0Base::BOOLEAN:
			val = (AMF0Base*) new AMF0BooleanData();
			break;
		case AMF0Base::STRING:
			val = (AMF0Base*) new AMF0StringData();
			break;
		case AMF0Base::OBJECT:
			val = (AMF0Base*) new AMF0ObjectData();
			break;
		case AMF0Base::REF:
			val = (AMF0Base*) new AMF0ReferenceData();
			break;
		case AMF0Base::ECMA_ARRAY:
			val = (AMF0Base*) new AMF0ECMAArrayData();
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
			break;
		case AMF0Base::DATE:
			val = (AMF0Base*) new AMF0DateData();
			break;
		case AMF0Base::LSTRING:
			val = (AMF0Base*) new AMF0LongStringData();
			break;
		}
		if(val) {
			sz += val->deserialize(ctx, stream);
			cdsl_slistPutTail(this,val);
		}
	}
	return sz;
}


Iterator<AMF0Base>* AMF0::iterator() {
	return new AMF0Iterator(this,is_mutable);
}


int AMF0::add(AMF0Base* obj)
{
	if(is_mutable)
		return cdsl_slistPutTail(this,obj);
	return FALSE;
}

void AMF0::remove(AMF0Base* obj)
{
	if(is_mutable)
		cdsl_slistRemove(this, obj);
}

void AMF0::clear() {
	while(cdsl_slistIsEmpty(this))
	{
		AMF0Base* obj = (AMF0Base*) cdsl_slistDequeue(this);
		delete obj;
	}
}


void AMF0::setMutable(bool is_mutable)
{
	this->is_mutable = is_mutable;
}



int AMF0::length(void)
{
	return cdsl_slistSize(this);
}


AMF0NumberData::AMF0NumberData()
{
	val = 0.0;
}

AMF0NumberData::~AMF0NumberData()
{

}

ssize_t AMF0NumberData::serialize(void* ctx, MediaStream* stream)
{
	if(!stream)
		return -1;
	amf0_val obj;
	obj.marker = (uint8_t) AMF0Base::NUMBER;
	obj.numv = val;
	return stream->write((uint8_t*) &obj, offsetof(amf0_val, numv) + sizeof(obj.numv));
}

ssize_t AMF0NumberData::serialize(void* ctx, uint8_t* into) {
	if(!into)
		return -1;

	amf0_val obj;
	obj.marker = (uint8_t) AMF0Base::NUMBER;
	obj.numv = val;
	size_t wsz = offsetof(amf0_val, numv) + sizeof(obj.numv);
	memcpy(into, &obj, wsz);
	return wsz;
}

ssize_t AMF0NumberData::deserialize(void* ctx, const MediaStream* stream) {
	if(!stream)
		return -1;
	return stream->read((uint8_t*) &val,sizeof(double));
}


ssize_t AMF0NumberData::deserialize(void* ctx, const uint8_t* from) {
	if(!from)
		return -1;
	size_t sz = sizeof(double);
	memcpy(&val,from,sz);
	return sz;
}

AMF0Base::AMF0Type AMF0NumberData::getType()
{
	return AMF0Base::NUMBER;
}

double AMF0NumberData::getValue()
{
	return val;
}

AMF0BooleanData::AMF0BooleanData()
{
	val = false;
}

AMF0BooleanData::~AMF0BooleanData() {
}

ssize_t AMF0BooleanData::serialize(void* ctx, MediaStream* stream) {
	if(!stream)
		return -1;
	amf0_val obj;
	obj.boolv = val;
	obj.marker = (uint8_t) AMF0Base::BOOLEAN;
	return stream->write((uint8_t*) &obj, offsetof(amf0_val, boolv) + sizeof(obj.boolv));
}

ssize_t AMF0BooleanData::serialize(void* ctx, uint8_t* into) {
	if(!into)
		return -1;

	amf0_val obj;
	obj.boolv = val;
	obj.marker = (uint8_t) AMF0Base::BOOLEAN;
	size_t sz = offsetof(amf0_val, boolv) + sizeof(obj.boolv);
	memcpy(into, &obj, sz);
	return sz;
}

ssize_t AMF0BooleanData::deserialize(void* ctx, const MediaStream* stream) {
	if(!stream)
		return -1;
	uint8_t v;
	stream->read((uint8_t*) &v, sizeof(uint8_t));
	val = v == 1? true : false;
	return sizeof(uint8_t);
}

ssize_t AMF0BooleanData::deserialize(void* ctx, const uint8_t* from) {
	if(!from)
		return -1;
	val = *from == 1? true : false;
	return sizeof(uint8_t);
}

AMF0Base::AMF0Type AMF0BooleanData::getType()
{
	return AMF0Base::BOOLEAN;
}

bool AMF0BooleanData::getValue()
{
	return val;
}


AMF0StringData::AMF0StringData() {
	val.clear();
}

AMF0StringData::~AMF0StringData() {
}

ssize_t AMF0StringData::serialize(void* ctx, MediaStream* stream) {
	if(!stream)
		return -1;
	amf0_val obj;
	size_t sz;
	obj.marker = (uint8_t) AMF0Base::STRING;
	obj.strv.len = val.length();
	sz = stream->write((uint8_t*) &obj,offsetof(amf0_val, strv.len) + sizeof(obj.strv.len));
	sz += stream->write((uint8_t*)val.c_str(), obj.strv.len);
	return sz;
}

ssize_t AMF0StringData::serialize(void* ctx, uint8_t* into) {
	if(!into)
		return -1;
	amf0_val obj;
	obj.marker = (uint8_t) AMF0Base::STRING;
	obj.strv.len = val.length();
	size_t sz = offsetof(amf0_val, strv.len) + sizeof(obj.strv.len);
	memcpy(into, &obj, sz);
	memcpy(into, val.c_str(), obj.strv.len);
	return sz + obj.strv.len;
}

ssize_t AMF0StringData::deserialize(void* ctx, const MediaStream* stream) {
	if(!stream)
		return -1;
	amf0_val obj;
	size_t sz;
	sz = stream->read((uint8_t*) &obj.strv.len,sizeof(obj.strv.len));
	// TODO: have to consider endianess difference between AMF0 and platform
	sz += obj.strv.len;

	val.resize(obj.strv.len);
	while(obj.strv.len--){
		val.push_back(stream->read());
	}
	return sz;
}

ssize_t AMF0StringData::deserialize(void* ctx, const uint8_t* from) {
	if(!from)
		return -1;
	amf0_val obj;
	size_t sz = sizeof(obj.strv.len);
	memcpy(&obj.strv.len, from, sizeof(obj.strv.len));

	val.resize(obj.strv.len);
	char* c = (char*) &from[sz];
	sz += obj.strv.len;

	while(obj.strv.len--){
		val.push_back(*c++);
	}
	return sz;
}

AMF0Base::AMF0Type AMF0StringData::getType() {
	return AMF0Base::STRING;
}

const std::string* AMF0StringData::getValue() {
	return &val;
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
