/*
 * mpipe_test.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: innocentevil
 */

#include "FLVTag.h"
#include "AMF0.h"


int main(void)
{
	MediaPipe::AMF0* amf0 = new MediaPipe::AMF0();
	MediaPipe::AMF0BooleanData* bdata = new MediaPipe::AMF0BooleanData();
	amf0->add(bdata);
	MediaPipe::AMF0::AMF0Base* base = (MediaPipe::AMF0::AMF0Base*) bdata;
//	base->serialize(NULL,NULL);
}
