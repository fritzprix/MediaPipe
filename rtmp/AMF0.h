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
#include <pthread.h>
#include "Iterator.h"
#include "core/MediaStream.h"
#include "core/Serialize.h"
#include "cdsl_slist.h"
#include "cdsl_dlist.h"
#include "cdsl_hashtree.h"


namespace MediaPipe {

class AMF0Base : public Serializable<void> ,slistNode_t{
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

class AMF0Iterable : public Iterable<AMF0Base> , public slistEntry_t {
public:
	AMF0Iterable();
	virtual ~AMF0Iterable() {};
};


class AMF0 : Serializable<void>, AMF0Iterable {
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
		AMF0Iterator(AMF0Iterable* iterable, bool is_mutable);
		~AMF0Iterator();
		bool hasNext();
		void reset();
		AMF0Base* next();
		void remove();
		bool lock();
		void unlock();
	private:
		pthread_mutex_t mtx_lock;
		bool is_mutable;
		AMF0Iterable* iterable;
	};

	/**
	 * for memory management reason, AMF0 can be either mutable or immutable.
	 * it should be explicit that how the each AMF0type item is created in memory perspective.
	 * if it is serialized from MeidaStream, the items in AMF0 object will be allocated and deallocated
	 * automatically by AMF0. otherwise, those who allocate items has also responsibility to deallocate them.
	 *
	 * so mutable means user can have control over the items
	 */

	AMF0(bool is_mutable) : iter(this,is_mutable) , is_mutable(is_mutable) {  };
	AMF0() : iter(this,true), is_mutable(true) { };
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
	AMF0Iterator iter;
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



class AMF0Property : public AMF0::AMF0Data<const AMF0Base*> , public hashNode_t , public dlistNode_t {
public:
	AMF0Property();
	~AMF0Property();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	const AMF0Base* getValue();
	const std::string* getName();
private:
	AMF0StringData propName;
	AMF0Base* propData;
};

class AMF0PropertyIterable : public Iterable<AMF0Property> , public dlistEntry_t {
public :
	AMF0PropertyIterable ();
	virtual ~AMF0PropertyIterable(){};
};

class AMF0PropertyIterator : public Iterator<AMF0Property> , public listIter_t {
public:
	AMF0PropertyIterator(AMF0PropertyIterable* iterable, bool is_mutable);
	virtual ~AMF0PropertyIterator();
	void reset();
	bool hasNext();
	AMF0Property* next();
	void remove();
	bool lock();
	void unlock();
private:
	bool isMutable;
	pthread_mutex_t mtx_lock;
	AMF0PropertyIterable* iterable;
};


class AMF0ObjectData : public AMF0PropertyIterable, public AMF0::AMF0Data<size_t> , public hashRoot_t  {
public:
	AMF0ObjectData(bool is_mutable);
	~AMF0ObjectData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	size_t getValue();
	Iterator<AMF0Property>* iterator();
	void addProperty(AMF0Property* prop);
	void removeProperty(AMF0Property* prop);
	AMF0Property* get(const char* key);
private:
	bool isMutable;
	AMF0PropertyIterator iter;
};

class AMF0ReferenceData : public AMF0::AMF0Data<uint16_t> {
public:
	AMF0ReferenceData();
	~AMF0ReferenceData(){};
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	uint16_t getValue();
private:
	uint16_t ref_offset;
};

class AMF0ECMAArrayData : public AMF0PropertyIterable, public AMF0::AMF0Data<size_t>, public hashRoot_t {
public:
	AMF0ECMAArrayData(bool is_mutable);
	~AMF0ECMAArrayData();
	ssize_t serialize(void* ctx, MediaStream* stream) ;
	ssize_t serialize(void* ctx, uint8_t* into);
	ssize_t deserialize(void* ctx, const MediaStream* stream);
	ssize_t deserialize(void* ctx, const uint8_t* from);
	AMF0Base::AMF0Type getType();
	size_t getValue();
	Iterator<AMF0Property>* iterator();
	void addProperty(AMF0Property* prop);
	void removeProperty(AMF0Property* prop);
	AMF0Property* get(const char* key);
private:
	bool isMutable;
	AMF0PropertyIterator iter;
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
