/*
 * MediaStreamTestUnit.h
 *
 *  Created on: Mar 29, 2016
 *      Author: innocentevil
 */

#ifndef CORE_STREAM_MEDIASTREAMTESTUNIT_H_
#define CORE_STREAM_MEDIASTREAMTESTUNIT_H_

#include "test/TestUnit.h"
#include "MediaClientSocketStream.h"
#include "MediaServerSocketStream.h"

namespace MediaPipe {

class MediaStreamTestUnit : public TestUnit {
public:
	MediaStreamTestUnit();
	virtual ~MediaStreamTestUnit();
	bool performTest();
private:
	MediaClientSocketStream* client;
	MediaServerSocketStream* server;
	pthread_t client_thread;
	pthread_t server_thread;
};

} /* namespace MediaPipe */

#endif /* CORE_STREAM_MEDIASTREAMTESTUNIT_H_ */
