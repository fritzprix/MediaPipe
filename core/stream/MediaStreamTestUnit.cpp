/*
 * MediaStreamTestUnit.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: innocentevil
 */

#include "MediaStreamTestUnit.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void* server_routine(void* arg);
static void* client_routine(void* arg);

namespace MediaPipe {


MediaStreamTestUnit::MediaStreamTestUnit() {
	client = new MediaClientSocketStream(3030);
	server = new MediaServerSocketStream(3030);
	memset(&server_thread,0,sizeof(pthread_t));
	memset(&client_thread,0,sizeof(pthread_t));

}

MediaStreamTestUnit::~MediaStreamTestUnit() {
	delete client;
	delete server;
}

bool MediaStreamTestUnit::performTest() {
	pthread_create(&server_thread,NULL,server_routine,server);
	pthread_create(&client_thread,NULL,client_routine,client);


	pthread_join(server_thread,NULL);
	pthread_join(client_thread,NULL);
	return TRUE;
}

} /* namespace MediaPipe */


static void* server_routine(void* arg)
{
	uint8_t msg_buffer[100];
	MediaPipe::MediaServerSocketStream* server = (MediaPipe::MediaServerSocketStream*) arg;
	server->open();
	server->read(msg_buffer,100);
	printf("I got a msg :  (%s) from client\n",msg_buffer);
	server->write(msg_buffer,sprintf((char*) msg_buffer,"Hello Client !!"));
	server->close();
	return arg;
}

static void* client_routine(void* arg)
{
	uint8_t msg_buffer[100];
	MediaPipe::MediaClientSocketStream* client = (MediaPipe::MediaClientSocketStream*) arg;
	client->open();
	client->write((const uint8_t*) msg_buffer, sprintf((char*) msg_buffer,"Hello Server !!"));
	client->read(msg_buffer,100);
	printf("I got a msg : (%s) from server\n",msg_buffer);
	client->close();
	return arg;
}
