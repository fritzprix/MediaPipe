/*
 * Serialize.h
 *
 *  Created on: Mar 24, 2016
 *      Author: innocentevil
 */

#ifndef CORE_SERIALIZE_SERIALIZE_H_
#define CORE_SERIALIZE_SERIALIZE_H_


namespace MediaPipe {

template<class T>
class Serializable {
public:
	Serializable() {};
	virtual ~Serializable() {};
	virtual ssize_t serialize(T* ctx, MediaStream* stream) = 0;
	virtual ssize_t serialize(T* ctx, uint8_t* into) = 0;
	virtual ssize_t deserialize(T* ctx, const MediaStream* stream) = 0;
	virtual ssize_t deserialize(T* ctx, const uint8_t* from) = 0;
};


template <class T>
class Payload : Serializable<T> {
public:
	Payload(){};
	virtual ~Payload(){};
};

template <class T>
class PayloadEventHandler {
public:
	PayloadEventHandler() {};
	virtual ~PayloadEventHandler(){};
	virtual void onPayload(const Payload<T>* payload,const MediaStream* stream);
	virtual void onPayload(const Payload<T>* payload,const uint8_t* buffer);
};

template <class T>
class Unpackable {
public:
	Unpackable() {};
	virtual ~Unpackable() {};
	virtual Payload<T>* getPayload() = 0;
};

template <class T>
class Packable {
public:
	Packable() {};
	virtual ~Packable(){};
	virtual bool setPayload(const Payload<T>* payload) = 0;
};


}

#endif /* CORE_SERIALIZE_SERIALIZE_H_ */
