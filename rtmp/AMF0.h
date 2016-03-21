/*
 * AMF0.h
 *
 *  Created on: Mar 19, 2016
 *      Author: innocentevil
 */


#ifndef RTMP_AMF0_H_
#define RTMP_AMF0_H_

#include "core/MediaStream.h"
#include "cdsl_dlist.h"

namespace MediaPipe {

class AMF0 : MediaStream::Serializable {
public:
	class AMF0Object : MediaStream::Serializable {
	public:
		AMF0Object();
		virtual ~AMF0Object();
		virtual size_t getSize(void) = 0;
		virtual ssize_t read(uint8_t* data) = 0;
	private:
		dlistNode_t	list_node;
	};

	AMF0();
	virtual ~AMF0();
	ssize_t serialize(MediaContext* meta, MediaStream* stream);
	ssize_t deserialize(MediaContext* meta, MediaStream* stream);

	ssize_t read(uint8_t* data);
	void add(AMF0Object* obj);
	AMF0Object* remove(int idx);
	int lenght(void);
	void remove(AMF0Object* obj);

};


} /* namespace MediaPipe */

#endif /* RTMP_AMF0_H_ */
