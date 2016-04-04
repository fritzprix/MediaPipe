/*
 * AMF0.h
 *
 *  Created on: Mar 19, 2016
 *      Author: innocentevil
 */


#ifndef RTMP_AMF0_H_
#define RTMP_AMF0_H_

#include <string>
#include <stdint.h>
#include "mpipe.h"
#include "core/MediaStream.h"
#include "core/Serialize.h"
#include "cdsl_slist.h"

namespace MediaPipe {

class AMF0Base : Serializable<void> ,slistNode_t{
	friend class AMF0;
public:
	typedef enum {
		NUMBER = 0,
		BOOLEAN = 1,
		STRING = 2,
		OBJECT = 3,
		MV_CLIP = 4,
		NUL = 5,
		UND = 6,
		REF = 7,
		ECMA_ARRAY = 8,
		OBJ_END = 9,
		STRICT_ARRAY = 10,
		DATE = 11,
		LSTRING = 12
	}AMF0Type;

	AMF0Base();
	virtual ~AMF0Base();
	virtual AMF0Type getType() = 0;
};

class AMF0 : Serializable<void>, Iterable<AMF0Base>, slistEntry_t {
public:
	template <class T>
	class AMF0Data : public AMF0Base {
	public:
		AMF0Data();
		virtual ~AMF0Data() {};
		virtual T getValue() = 0;
	};

	class AMF0Iterator :public Iterator<AMF0Base> , listIter_t {
	public:
		AMF0Iterator(AMF0* amf0, bool is_mutable);
		~AMF0Iterator();
		bool hasNext();
		AMF0Base* next();
		void remove();
	private:
		bool is_mutable;
	};

	/**
	 * for memory management reason, AMF0 can be either mutable or immutable.
	 * it should be explicit that how the each AMF0type item is created in memory perspective.
	 * if it is serialized from MeidaStream, the items in AMF0 object will be allocated and deallocated
	 * automatically by AMF0. otherwise, those who allocate items has also responsibility to deallocate them.
	 *
	 * so mutable means user can have control over the items
	 */

	AMF0(bool is_mutable);
	AMF0();
	virtual ~AMF0();

	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	Iterator<AMF0Base>* iterator();

	int add(AMF0Base* obj);
	void remove(AMF0Base* obj);
	void clear();
	void setMutable(bool is_mutable);
	int length(void);
private:
	bool is_mutable;
};

class AMF0NumberData : public AMF0::AMF0Data<double> {
public:
	AMF0NumberData();
	~AMF0NumberData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	double getValue();
	AMF0Base::AMF0Type getType();
private:
	double val;
};

class AMF0BooleanData : public AMF0::AMF0Data<bool> {
public:
	AMF0BooleanData();
	~AMF0BooleanData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	bool getValue();
private:
	bool val;
};

class AMF0StringData : public AMF0::AMF0Data<const std::string*> {
public:
	AMF0StringData();
	~AMF0StringData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	const std::string* getValue();
private:
	std::string val;
};

class AMF0ObjectData : public AMF0::AMF0Data<const AMF0*> {
public:
	AMF0ObjectData();
	~AMF0ObjectData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	const AMF0* getValue();
};

class AMF0ReferenceData : public AMF0::AMF0Data<uint16_t> {
public:
	AMF0ReferenceData();
	~AMF0ReferenceData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	uint16_t getValue();

};

class AMF0ECMAArrayData : public AMF0::AMF0Data<const AMF0*> {
public:
	AMF0ECMAArrayData();
	~AMF0ECMAArrayData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	const AMF0* getValue();

};

class AMF0StrictArrayData : public AMF0::AMF0Data<const AMF0*> {
public:
	AMF0StrictArrayData();
	~AMF0StrictArrayData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	const AMF0* getValue();

};

class AMF0DateData : public AMF0::AMF0Data<time_t> {
public:
	AMF0DateData();
	~AMF0DateData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	time_t getValue();
};

class AMF0LongStringData : public AMF0::AMF0Data<const std::string*> {
public:
	AMF0LongStringData();
	~AMF0LongStringData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	const std::string* getValue();
};




} /* namespace MediaPipe */

template<class T>
inline MediaPipe::AMF0::AMF0Data<T>::AMF0Data() {
}

#endif /* RTMP_AMF0_H_ */
