/*
 * MediaStreamTestUnit.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: innocentevil
 */

#include "MediaStreamTestUnit.h"
#include <stdlib.h>
#include <stdio.h>

namespace MediaPipe {

MediaStreamTestUnit::MediaStreamTestUnit() {
	client = new MediaClientSocketStream(3030);
	server = new MediaServerSocketStream(3030);
}

MediaStreamTestUnit::~MediaStreamTestUnit() {
	delete client;
	delete server;
}

bool MediaStreamTestUnit::performTest() {
	uint8_t server_msg[100];
	uint8_t client_msg[100];
	server->open();
	client->open();
//	client->write((const uint8_t*) client_msg, ::sprintf((char*) client_msg,"Hello Server! %d\n",0));
//	ssize_t rsz = server->read(server_msg,100);
//	::printf("rsz : %d\n",rsz);
	client->close();
}

} /* namespace MediaPipe */
