/*
 * AMF0.cpp
 *
 *  Created on: Mar 19, 2016
 *      Author: innocentevil
 */


#include "cdsl_slist.h"
#include "cdsl_dlist.h"
#include "cdsl_hashtree.h"
#include "Iterator.h"
#include "AMF0.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <FLVTag.h>
#include <iostream>
#include <functional>
#include <string>
#include <map>


typedef struct {
	uint8_t _[3];
}__attribute__((packed)) uint24_t;

struct amf0_str {
	uint16_t 		len;
	char			str_seq;
} __attribute__((packed));

struct amf0_obj_prop {
	hashNode_t           node;
	MediaPipe::AMF0Base* obj;
	amf0_str		     key;
}__attribute__((packed));

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
		struct amf0_lstr lstrv;
		uint16_t ref;
		uint32_t prop_cnt;
		struct amf0_date date;
	} __attribute__((packed));
} __attribute__((packed));




namespace MediaPipe {

static AMF0Base* parse_amf0(void* ctx, const MediaStream* stream,size_t* sz);
static AMF0Base* parse_amf0(void* ctx, const uint8_t* buffer, size_t* sz);
static bool __is_big_endian(void);
/*
 * AMF0 has big endian byte stream and it can be different from the native byte order.
 * however, the octect sized type has no problem with it. (e.g. character string), when even the byte order is different.
 * the problematic case is only the size of type is larger than octet. but even here, not all the byte stream should be bswapped.
 * because we do actually use just part of them. for example, the lenght of packet should be bswapped when parsing the whole packet.
 * but after the moment, it's barely used. from this kind of practical use case, we can optimize the performance, by reducing the usage of bswap.
 *
 * basically all the binary is saved in big endian during deserialization and serialization.
 * however, it will be bswapped on demand. (for example, in getter / setter of the instance or parsing)
 *
 * how-to-detect-native byteorder
 * I don't like the approach that branches code with some macro variable which is fixed in build time.
 * it's less readable, and make source code look spaghetti.
 * I prefer detect it at runtime with investigating int or short type
 * it makes C look like C more
 */
static bool IS_BIG_ENDIAN = __is_big_endian();

AMF0Base::AMF0Base()
{
	cdsl_slistNodeInit(this);
}

AMF0Base::~AMF0Base()
{
}


AMF0Iterable::AMF0Iterable() {
	cdsl_slistEntryInit(this);
}


AMF0Iterator::AMF0Iterator(AMF0Iterable* iterable,bool is_mutable) {
	cdsl_slistIterInit(iterable,this);
	this->is_mutable = is_mutable;
	this->iterable = iterable;
	pthread_mutexattr_t lock_attr;
	pthread_mutexattr_init(&lock_attr);
	pthread_mutexattr_settype(&lock_attr,PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&mtx_lock,&lock_attr);
}

AMF0Iterator::~AMF0Iterator() {
	pthread_mutex_destroy(&mtx_lock);
}

bool AMF0Iterator::hasNext() {
	return cdsl_iterHasNext(this);
}

void AMF0Iterator::reset() {
	cdsl_slistIterInit(iterable, this);
}


AMF0Base* AMF0Iterator::next() {
	return (AMF0Base*) cdsl_iterNext(this);
}

void AMF0Iterator::remove() {
	if(!is_mutable)
		return;
	cdsl_iterRemove(this);
}


bool AMF0Iterator::lock()
{
	if(pthread_mutex_lock(&mtx_lock) == EXIT_SUCCESS)
		return true;
	return false;
}

void AMF0Iterator::unlock()
{
	pthread_mutex_unlock(&mtx_lock);
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

	FLVCommonTag* flv_t = (FLVCommonTag*) ctx;
	AMF0Base *val = NULL;
	size_t t_sz = flv_t->getSize();
	size_t sz = 0;
	while(sz <= t_sz)
	{
		val = parse_amf0(ctx, (uint8_t*) &from[sz],&sz);
		if(val) {
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
	FLVCommonTag* flv_t = (FLVCommonTag*) ctx;
	size_t t_sz = flv_t->getSize();
	size_t sz = 0;
	while(sz <= t_sz)
	{
		val = parse_amf0(ctx, stream,&sz);
		if(val) {
			cdsl_slistPutTail(this,val);
		}
	}
	return sz;
}

Iterator<AMF0Base>* AMF0::iterator() {
	if(!iter.lock())
		return NULL;
	iter.reset();
	return &iter;
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
	if(!IS_BIG_ENDIAN)
	{
		/*
		 *  in case of platform byte order is different from AMF0 byte order(be)
		 *  byte swap performed
		 */
		switch(sizeof(double)){
		case 4:
			return __bswap_32(val);
		case 8:
			return __bswap_64(val);
		}
	}
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
	if(!IS_BIG_ENDIAN)
	{
		/*
		 * native byte order is different from AMF0
		 */
		obj.strv.len = __bswap_16((uint16_t) val.length());
	}
	else
	{
		obj.strv.len = val.length();
	}
	sz = stream->write((uint8_t*) &obj,offsetof(amf0_val, strv.len) + sizeof(obj.strv.len));
	sz += stream->write((uint8_t*)val.c_str(), obj.strv.len);
	return sz;
}

ssize_t AMF0StringData::serialize(void* ctx, uint8_t* into) {
	if(!into)
		return -1;
	amf0_val obj;
	obj.marker = (uint8_t) AMF0Base::STRING;
	if(!IS_BIG_ENDIAN)
	{
		obj.strv.len = __bswap_16((uint16_t) val.length());
	}
	else
	{
		obj.strv.len = val.length();
	}
	size_t sz = offsetof(amf0_val, strv.len) + sizeof(obj.strv.len);
	memcpy(into, &obj, sz);
	memcpy(into, val.c_str(), val.length());
	return sz + obj.strv.len;
}

ssize_t AMF0StringData::deserialize(void* ctx, const MediaStream* stream) {
	if(!stream)
		return -1;
	amf0_val obj;
	size_t sz;
	sz = stream->read((uint8_t*) &obj.strv.len,sizeof(obj.strv.len));
	if(obj.strv.len == 0)
		return sz;
	if(!IS_BIG_ENDIAN)
	{
		obj.strv.len = __bswap_16(obj.strv.len);
	}

	val.resize(obj.strv.len);
	while(obj.strv.len--)
	{
		val.push_back(stream->read());
	}
	sz += obj.strv.len;
	return sz;
}

ssize_t AMF0StringData::deserialize(void* ctx, const uint8_t* from) {
	if(!from)
		return -1;
	amf0_val obj;
	size_t sz = sizeof(obj.strv.len);
	memcpy(&obj.strv.len, from, sizeof(obj.strv.len));
	if(obj.strv.len == 0)
		return sz;
	if(!IS_BIG_ENDIAN)
	{
		obj.strv.len = __bswap_16(obj.strv.len);
	}

	val.resize(obj.strv.len);
	char* c = (char*) &from[sz];
	while(obj.strv.len--){
		val.push_back(*c++);
	}
	sz += obj.strv.len;
	return sz;
}

AMF0Base::AMF0Type AMF0StringData::getType() {
	return AMF0Base::STRING;
}

const std::string* AMF0StringData::getValue() {
	return &val;
}


AMF0Property::AMF0Property() {
	propData = NULL;
	cdsl_dlistNodeInit(this);
	cdsl_hashtreeNodeInit(this,propName.getValue()->c_str());
}

AMF0Property::~AMF0Property() {
	if(propData)
		delete propData;
}

ssize_t AMF0Property::serialize(void* ctx, MediaStream* stream)
{
	if(!stream)
		return -1;
	return propName.serialize(ctx, stream) + propData->serialize(ctx, stream);
}

ssize_t AMF0Property::serialize(void* ctx, uint8_t* into)
{
	if(!into)
		return -1;
	return propName.serialize(ctx, into) + propData->serialize(ctx, into);
}

ssize_t AMF0Property::deserialize(void* ctx, const MediaStream* stream)
{
	if(!stream)
		return -1;
	size_t sz = propName.deserialize(ctx,stream);
	propData = parse_amf0(ctx, stream, &sz);
	if(!propData)
		return -1;
	cdsl_hashtreeNodeInit(this, propName.getValue()->c_str());
	return sz;
}

ssize_t AMF0Property::deserialize(void* ctx, const uint8_t* from)
{
	if(!from)
		return -1;
	size_t sz = propName.deserialize(ctx, from);
	propData = parse_amf0(ctx, from, &sz);
	if(!propData)
		return -1;
	cdsl_hashtreeNodeInit(this,propName.getValue()->c_str());
	return sz;
}

AMF0Base::AMF0Type AMF0Property::getType()
{
	if(propData)
		return propData->getType();
	return AMF0Base::OBJ_END;
}

const AMF0Base* AMF0Property::getValue()
{
	return propData;
}

const std::string* AMF0Property::getName()
{
	return propName.getValue();
}

AMF0PropertyIterable::AMF0PropertyIterable ()
{
	cdsl_dlistEntryInit(this);
}

AMF0PropertyIterator::AMF0PropertyIterator(AMF0PropertyIterable* iterable, bool is_mutable)
{
	cdsl_dlistIterInit(iterable,this);
	this->iterable = iterable;
	isMutable = is_mutable;
	pthread_mutexattr_t lock_attr;
	pthread_mutexattr_init(&lock_attr);
	pthread_mutexattr_settype(&lock_attr, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init(&mtx_lock, &lock_attr);
}

AMF0PropertyIterator::~AMF0PropertyIterator()
{
	pthread_mutex_destroy(&mtx_lock);
}


void AMF0PropertyIterator::reset()
{
	cdsl_dlistIterInit(iterable, this);
}

bool AMF0PropertyIterator::hasNext()
{
	return cdsl_dlistIterHasNext(this);
}

AMF0Property* AMF0PropertyIterator::next()
{
	return (AMF0Property*) cdsl_dlistIterNext(this);
}

void AMF0PropertyIterator::remove()
{
	if(!isMutable)
		return;
	cdsl_dlistIterRemove(this);
}


bool AMF0PropertyIterator::lock()
{
	if(pthread_mutex_lock(&mtx_lock) == EXIT_SUCCESS)
		return true;
	return false;
}

void AMF0PropertyIterator::unlock()
{
	pthread_mutex_unlock(&mtx_lock);
}


AMF0ObjectData::AMF0ObjectData(bool is_mutable) : iter(this,is_mutable) {
	cdsl_hashtreeRootInit(this);
	isMutable = is_mutable;
}

AMF0ObjectData::~AMF0ObjectData() {
	AMF0Property* prop;
	if(!isMutable) {
		while((prop = (AMF0Property*) cdsl_dlistDequeue(this)))
		{
			delete prop;
		}
	}
}

ssize_t AMF0ObjectData::serialize(void* ctx, MediaStream* stream) {
	if(!stream)
		return -1;
	AMF0PropertyIterator iterator(this, isMutable);
	AMF0Property *prop;
	size_t sz = stream->write((uint8_t) AMF0Base::OBJECT);
	while(iterator.hasNext())
	{
		prop = (AMF0Property*) iterator.next();
		if(!prop)
			return -1;
		sz += prop->serialize(ctx,stream);
	}
	return sz;
}

ssize_t AMF0ObjectData::serialize(void* ctx, uint8_t* into) {
	if(!into)
		return -1;
	AMF0PropertyIterator iterator(this,isMutable);
	AMF0Property* prop;
	*into = AMF0Base::OBJECT;
	size_t sz = sizeof(uint8_t);
	while(iterator.hasNext())
	{
		prop = (AMF0Property*) iterator.next();
		if(!prop)
			return -1;
		sz += prop->serialize(ctx, &into[sz]);
	}
	return sz;
}


ssize_t AMF0ObjectData::deserialize(void* ctx, const MediaStream* stream) {
	if(!stream)
		return -1;
	size_t sz = 0;
	AMF0Property* prop;
	do {
		prop = new AMF0Property();
		sz += prop->deserialize(ctx, stream);
		cdsl_dlistPutTail(this, prop);
		cdsl_hashtreeInsert(this, prop);
	} while(prop->getType() != OBJ_END);
	return sz;
}

ssize_t AMF0ObjectData::deserialize(void* ctx, const uint8_t* from) {
	if(!from)
		return -1;
	size_t sz = 0;
	AMF0Property* prop;
	do {
		prop = new AMF0Property();
		sz += prop->deserialize(ctx, from);
		cdsl_dlistPutTail(this, prop);
		cdsl_hashtreeInsert(this, prop);
	}while(prop->getType() != OBJ_END);
	return sz;
}

AMF0Base::AMF0Type AMF0ObjectData::getType()
{
	return AMF0Base::OBJECT;
}

size_t AMF0ObjectData::getValue()
{
	return cdsl_dlistSize(this);
}

Iterator<AMF0Property>* AMF0ObjectData::iterator()
{
	if(!iter.lock())
		return NULL;

	iter.reset();
	return &iter;
}

void AMF0ObjectData::addProperty(AMF0Property* prop)
{
	if(!isMutable)
		return;
	if(!iter.lock())
		return;
	cdsl_dlistPutTail(this, prop);
	iter.unlock();
	cdsl_hashtreeInsert(this, prop);
}

void AMF0ObjectData::removeProperty(AMF0Property* prop)
{
	if(!isMutable)
		return;
	if(!iter.lock())
		return;
	cdsl_dlistRemove(prop);
	iter.unlock();
	cdsl_hashtreeRemove(this,prop->getName()->c_str());
}

AMF0Property* AMF0ObjectData::lookupProperty(const char* key)
{
	return (AMF0Property*) cdsl_hashtreeLookup(this, key);
}

AMF0ReferenceData::AMF0ReferenceData() {
	ref_offset = 0;
}

ssize_t AMF0ReferenceData::serialize(void* ctx, MediaStream* stream) {
	if(!stream)
		return -1;
	amf0_val obj;
	obj.marker = AMF0Base::REF;
	obj.ref = ref_offset;
	return stream->write(&obj, offsetof(amf0_val, ref) + sizeof(obj.ref));
}

ssize_t AMF0ReferenceData::serialize(void* ctx, uint8_t* into) {
	if(!into)
		return -1;
	amf0_val obj;
	size_t sz = offsetof(amf0_val, ref) + sizeof(obj.ref);
	obj.marker = AMF0Base::REF;
	obj.ref = ref_offset;
	memcpy(into, &obj, sz);
	return sz;
}

ssize_t AMF0ReferenceData::deserialize(void* ctx, const MediaStream* stream) {
	if(!stream)
		return -1;
	return stream->read(&ref_offset, sizeof(uint16_t));
}

ssize_t AMF0ReferenceData::deserialize(void* ctx, const uint8_t* from) {
	if(!from)
		return -1;
	memcpy(&ref_offset, from, sizeof(uint16_t));
	return sizeof(uint16_t);
}

AMF0Base::AMF0Type AMF0ReferenceData::getType()
{
	return AMF0Base::REF;
}

uint16_t AMF0ReferenceData::getValue()
{
	if(!IS_BIG_ENDIAN)
		return __bswap_16(ref_offset);
	return ref_offset;
}

AMF0ECMAArrayData::AMF0ECMAArrayData(bool is_mutable) : iter(this,is_mutable) {
	isMutable = is_mutable;
	cdsl_hashtreeRootInit(this);
}

AMF0ECMAArrayData::~AMF0ECMAArrayData() {
	AMF0Property* prop;
	if(!isMutable) {
		while((prop = (AMF0Property*) cdsl_dlistDequeue(this)))
		{
			delete prop;
		}
	}
}

ssize_t AMF0ECMAArrayData::serialize(void* ctx, MediaStream* stream) {
	if(!stream)
		return -1;
	amf0_val obj;
	obj.marker = AMF0Base::ECMA_ARRAY;
	obj.prop_cnt = cdsl_dlistSize(this);
	if(!IS_BIG_ENDIAN)
	{
		obj.prop_cnt = __bswap_32(obj.prop_cnt);
	}
	size_t sz = stream->write(&obj, offsetof(amf0_val, prop_cnt) + sizeof(obj.prop_cnt));
	AMF0PropertyIterator iterator(this,false);
	AMF0Property* prop;
	while(iterator.hasNext()) {
		prop = (AMF0Property*) iterator.next();
		if(!prop)
			return -1;
		sz += prop->serialize(ctx, stream);
	}
	return sz;
}

ssize_t AMF0ECMAArrayData::serialize(void* ctx, uint8_t* into) {
	if(!into)
		return -1;
	amf0_val obj;
	obj.marker = AMF0Base::ECMA_ARRAY;
	obj.prop_cnt = cdsl_dlistSize(this);
	if(!IS_BIG_ENDIAN)
	{
		obj.prop_cnt = __bswap_32(obj.prop_cnt);
	}

	size_t sz = sizeof(obj.prop_cnt) + offsetof(amf0_val, prop_cnt);
	memcpy(into, &obj, sz);
	AMF0PropertyIterator iterator(this,false);
	AMF0Property* prop;
	while(iterator.hasNext()) {
		prop = (AMF0Property*) iterator.next();
		if(!prop)
			return -1;
		sz += prop->serialize(ctx, into);
	}
	return sz;
}

ssize_t AMF0ECMAArrayData::deserialize(void* ctx, const MediaStream* stream) {
	if(!stream)
		return -1;
	uint32_t prop_cnt , i;
	size_t sz = stream->read(&prop_cnt, sizeof(uint32_t));
	if(!IS_BIG_ENDIAN)
	{
		prop_cnt = __bswap_32(prop_cnt);
	}
	AMF0Property* prop;
	for(i = 0; i < prop_cnt;i++) {
		prop = new AMF0Property();
		sz += prop->deserialize(ctx, stream);
		cdsl_dlistPutTail(this,prop);
		cdsl_hashtreeInsert(this, prop);
	}
	return sz;
}

ssize_t AMF0ECMAArrayData::deserialize(void* ctx, const uint8_t* from) {
	if(!from)
		return -1;
	uint32_t prop_cnt, i;
	memcpy(&prop_cnt, from, sizeof(uint32_t));
	size_t sz = sizeof(uint32_t);
	if(!IS_BIG_ENDIAN)
	{
		prop_cnt = __bswap_32(prop_cnt);
	}
	AMF0Property* prop;
	for(i = 0;i < prop_cnt;i++) {
		prop = new AMF0Property();
		sz += prop->deserialize(ctx, from);
		cdsl_dlistPutTail(this,prop);
		cdsl_hashtreeInsert(this, prop);
	}
	return sz;
}

AMF0Base::AMF0Type AMF0ECMAArrayData::getType()
{
	return AMF0Base::ECMA_ARRAY;
}

size_t AMF0ECMAArrayData::getValue()
{
	return cdsl_dlistSize(this);
}

Iterator<AMF0Property>* AMF0ECMAArrayData::iterator()
{
	if(!iter.lock())
		return NULL;
	iter.reset();
	return &iter;
}

void AMF0ECMAArrayData::addProperty(AMF0Property* prop)
{
	if(!prop)
		return;
	if(!isMutable)
		return;
	if(!iter.lock())
		return;
	cdsl_dlistPutTail(this, prop);
	iter.unlock();
	cdsl_hashtreeInsert(this, prop);
}

void AMF0ECMAArrayData::removeProperty(AMF0Property* prop)
{
	if(!prop)
		return;
	if(!isMutable)
		return;
	if(!iter.lock())
		return;
	cdsl_dlistRemove(prop);
	iter.unlock();
	cdsl_hashtreeRemove(this,prop->_str_key);
}

AMF0Property* AMF0ECMAArrayData::lookupProperty(const char* key)
{
	if(!key)
		return NULL;
	return (AMF0Property*) cdsl_hashtreeLookup(this,key);
}

AMF0StrictArrayData::AMF0StrictArrayData(bool is_mutable) : iter(this,is_mutable)
{
	isMutable = is_mutable;
}

AMF0StrictArrayData::~AMF0StrictArrayData()
{
	if(!isMutable) {
		AMF0Base* val;
		while(!cdsl_slistIsEmpty(this))
		{
			val = (AMF0Base*) cdsl_slistDequeue(this);
			delete val;
		}
	}
}

ssize_t AMF0StrictArrayData::serialize(void* ctx, MediaStream* stream)
{
	if(!stream)
		return -1;
	amf0_val hdr;
	hdr.marker = AMF0Base::STRICT_ARRAY;
	hdr.prop_cnt = cdsl_slistSize(this);
	if(!IS_BIG_ENDIAN)
	{
		hdr.prop_cnt = __bswap_32(hdr.prop_cnt);
	}
	size_t sz = stream->write(&hdr, sizeof(hdr.prop_cnt) + offsetof(amf0_val, prop_cnt));
	AMF0Iterator iterator(this,false);
	AMF0Base* val;
	while(iterator.hasNext())
	{
		val = (AMF0Base*) iterator.next();
		if(!val)
			return -1;
		sz += val->serialize(ctx, stream);
	}
	return sz;
}

ssize_t AMF0StrictArrayData::serialize(void* ctx, uint8_t* into)
{
	if(!into)
		return -1;
	amf0_val hdr;
	hdr.marker = AMF0Base::STRICT_ARRAY;
	hdr.prop_cnt = cdsl_slistSize(this);
	if(!IS_BIG_ENDIAN)
	{
		hdr.prop_cnt = __bswap_32(hdr.prop_cnt);
	}
	memcpy(into, &hdr, sizeof(hdr.prop_cnt) + offsetof(amf0_val, prop_cnt));
	size_t sz = sizeof(uint32_t);
	AMF0Iterator iterator(this,false);
	AMF0Base* val;
	while(iterator.hasNext())
	{
		val = (AMF0Base*) iterator.next();
		if(!val)
			return -1;
		sz += val->serialize(ctx, into);
	}
	return sz;
}

ssize_t AMF0StrictArrayData::deserialize(void* ctx, const MediaStream* stream)
{
	if(!stream)
		return -1;
	uint32_t len;
	size_t sz = stream->read(&len, sizeof(uint32_t));
	if(!IS_BIG_ENDIAN)
	{
		len = __bswap_32(len);
	}
	AMF0Base* val;
	while(len--)
	{
		val = parse_amf0(ctx,stream,&sz);
		if(!val)
			return -1;
		cdsl_slistPutHead(this, val);
	}
	return sz;
}

ssize_t AMF0StrictArrayData::deserialize(void* ctx, const uint8_t* from)
{
	if(!from)
		return -1;
	uint32_t len;
	memcpy(&len, from,sizeof(uint32_t));
	size_t sz = sizeof(uint32_t);
	if(!IS_BIG_ENDIAN)
	{
		len = __bswap_32(len);
	}
	AMF0Base* val;
	while(len--)
	{
		val = parse_amf0(ctx, from, &sz);
		if(!val)
			return -1;
		cdsl_slistPutHead(this, val);
	}
	return sz;
}

AMF0Base::AMF0Type AMF0StrictArrayData::getType()
{
	return AMF0Base::STRICT_ARRAY;
}

size_t AMF0StrictArrayData::getValue()
{
	return cdsl_slistSize(this);
}

Iterator<AMF0Base>* AMF0StrictArrayData::iterator()
{
	if(!iter.lock())
		return NULL;
	iter.reset();
	return &iter;
}

int AMF0StrictArrayData::addValue(AMF0Base* val)
{
	if(!isMutable || !val)
		return -1;
	if(!iter.lock())
		return -1;
	int pos = cdsl_slistPutTail(this,val);
	iter.unlock();
	return pos;
}

AMF0Base* AMF0StrictArrayData::removeValue(int pos)
{
	if(!isMutable)
		return NULL;
	if(!iter.lock())
		return NULL;
	AMF0Base* del = (AMF0Base*) cdsl_slistRemoveAt(this, pos);
	iter.unlock();
	return del;
}

void AMF0StrictArrayData::removeValue(AMF0Base* val)
{
	if(!isMutable || !val)
		return;
	if(!iter.lock())
		return;
	cdsl_slistRemove(this, val);
	iter.unlock();
}

AMF0DateData::AMF0DateData() : epochmil(0), utc_offset(0)
{
}

AMF0DateData::~AMF0DateData()
{
}

ssize_t AMF0DateData::serialize(void* ctx, MediaStream* stream)
{
	if(!stream)
		return -1;
	amf0_val val;
	val.marker = AMF0Base::DATE;
	val.date.epoch_mills = epochmil;
	val.date.utc_adj = utc_offset;
	return stream->write(&val, sizeof(val.date) + offsetof(amf0_val, date));
}

ssize_t AMF0DateData::serialize(void* ctx, uint8_t* into)
{
	if(!into)
		return -1;
	amf0_val val;
	size_t sz = sizeof(val.date) + offsetof(amf0_val,date);
	val.marker = AMF0Base::DATE;
	val.date.epoch_mills = epochmil;
	val.date.utc_adj = utc_offset;
	memcpy(into, &val, sz);
	return sz;
}

ssize_t AMF0DateData::deserialize(void* ctx, const MediaStream* stream)
{
	if(!stream)
		return -1;
	amf0_date date;
	size_t sz = stream->read(&date, sizeof(amf0_date));
	epochmil = date.epoch_mills;
	utc_offset = date.utc_adj;
	return sz;
}

ssize_t AMF0DateData::deserialize(void* ctx, const uint8_t* from)
{
	if(!from)
		return -1;
	amf0_date date;
	size_t sz = sizeof(amf0_date);
	memcpy(&date,from, sizeof(amf0_date));
	epochmil = date.epoch_mills;
	utc_offset = date.utc_adj;
	return sz;
}

AMF0Base::AMF0Type AMF0DateData::getType()
{
	return AMF0Type::DATE;
}

time_t AMF0DateData::getValue()
{
	time_t time = (time_t) epochmil;
	if(!IS_BIG_ENDIAN)
	{
		time = __bswap_64(time);
	}
	struct tm* gmt = localtime(&time);
	gmt->tm_gmtoff = utc_offset;
	if(!IS_BIG_ENDIAN)
	{
		gmt->tm_gmtoff = __bswap_16(gmt->tm_gmtoff);
	}
	return mktime(gmt);
}

AMF0LongStringData::AMF0LongStringData()
{
}

AMF0LongStringData::~AMF0LongStringData()
{
}

ssize_t AMF0LongStringData::serialize(void* ctx, MediaStream* stream)
{
	if(!stream)
		return -1;
	amf0_val hdr;
	hdr.marker = AMF0Base::LSTRING;
	hdr.lstrv.len = lstr.length();
	if(!IS_BIG_ENDIAN)
	{
		hdr.lstrv.len = __bswap_32(hdr.lstrv.len);
	}
	size_t sz = stream->write(&hdr, offsetof(amf0_val, lstrv) + sizeof(hdr.lstrv.len));
	sz += stream->write(lstr.c_str(), lstr.length());
	return sz;
}

ssize_t AMF0LongStringData::serialize(void* ctx, uint8_t* into)
{
	if(!into)
		return -1;
	amf0_val hdr;
	hdr.marker = AMF0Base::LSTRING;
	hdr.lstrv.len = lstr.length();
	if(!IS_BIG_ENDIAN)
	{
		hdr.lstrv.len = __bswap_32(hdr.lstrv.len);
	}
	size_t strl = lstr.length();
	size_t sz = offsetof(amf0_val,lstrv) + sizeof(hdr.lstrv.len);
	memcpy(into, &hdr, sz);
	sz += strl;
	memcpy(into, lstr.c_str(),strl);
	return sz;
}

ssize_t AMF0LongStringData::deserialize(void* ctx, const MediaStream* stream)
{
	if(!stream)
		return -1;
	uint32_t strl;
	size_t sz = stream->read(&strl, sizeof(uint32_t));
	if(!IS_BIG_ENDIAN)
	{
		strl = __bswap_32(strl);
	}
	char c;
	lstr.resize(strl);
	while(strl--) {
		c = (char) stream->read();
		if(c < 0)
			return -1;
		lstr.push_back(c);
		sz++;
	}
	return sz;
}

ssize_t AMF0LongStringData::deserialize(void* ctx, const uint8_t* from)
{
	if(!from)
		return -1;
	uint32_t strl;
	size_t sz = sizeof(uint32_t);
	memcpy(&strl, from, sz);
	if(!IS_BIG_ENDIAN)
	{
		strl = __bswap_32(strl);
	}
	char* c = (char*) &from[sz];
	lstr.resize(strl);
	while(strl--) {
		lstr.push_back(*c++);
		sz++;
	}
	return sz;
}

AMF0Base::AMF0Type AMF0LongStringData::getType()
{
	return AMF0Base::LSTRING;
}

const std::string* AMF0LongStringData::getValue()
{
	return &lstr;
}

static AMF0Base* parse_amf0(void* ctx,const MediaStream* stream,size_t* sz) {
	if(!stream)
		return NULL;

	AMF0Base::AMF0Type type;
	AMF0Base* val = NULL;
	if((*sz += stream->read((uint8_t*) &type,sizeof(uint8_t))) < 0)
		return NULL;
	switch(type) {
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
		val = (AMF0Base*) new AMF0ObjectData(false);
		break;
	case AMF0Base::REF:
		val = (AMF0Base*) new AMF0ReferenceData();
		break;
	case AMF0Base::ECMA_ARRAY:
		val = (AMF0Base*) new AMF0ECMAArrayData(false);
		break;
	case AMF0Base::OBJ_END:
		*sz += sizeof(uint8_t);
		break;
	case AMF0Base::UND:
	case AMF0Base::NUL:
	case AMF0Base::MV_CLIP:
		// unsupported type and assumed to be invalid data source
		assert(false);
		break;
	case AMF0Base::STRICT_ARRAY:
		val = (AMF0Base*) new AMF0StrictArrayData(false);
		break;
	case AMF0Base::DATE:
		val = (AMF0Base*) new AMF0DateData();
		break;
	case AMF0Base::LSTRING:
		val = (AMF0Base*) new AMF0LongStringData();
		break;
	}
	if(val) {
		*sz += val->deserialize(ctx, stream);
	}
	return val;
}

static AMF0Base* parse_amf0(void* ctx, const uint8_t* buffer, size_t* sz) {
	if(!buffer)
		return NULL;
	AMF0Base* val = NULL;
	AMF0Base::AMF0Type type;
	type = (AMF0Base::AMF0Type) *buffer;
	*sz += sizeof(uint8_t);
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
		val = new AMF0ObjectData(false);
		break;
	case AMF0Base::REF:
		val = new AMF0ReferenceData();
		break;
	case AMF0Base::ECMA_ARRAY:
		val = new AMF0ECMAArrayData(false);
		break;
	case AMF0Base::OBJ_END:
		*sz += sizeof(uint8_t);
		break;
	case AMF0Base::UND:
	case AMF0Base::NUL:
	case AMF0Base::MV_CLIP:
		// unsupported type and assumed to be invalid data source
		assert(false);
		break;
	case AMF0Base::STRICT_ARRAY:
		val = new AMF0StrictArrayData(false);
		break;
	case AMF0Base::DATE:
		val = new AMF0DateData();
		break;
	case AMF0Base::LSTRING:
		val = new AMF0LongStringData();
		break;
	}
	if(val) {
		*sz += val->deserialize(ctx, &buffer[1]);
	}
	return val;
}

static bool __is_big_endian(void)
{
	uint32_t tword;
	uint8_t* toctet = (uint8_t*) &tword;
	tword = 1;
	if(toctet[3])
		return true;
	return false;
}


} /* namespace MediaPipe */


