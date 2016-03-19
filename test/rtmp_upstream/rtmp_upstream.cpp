/*
 * mpipe_test.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: innocentevil
 */

#include "FLVTag.h"


int main(void)
{
	MediaPipe::FLVTag* flvTag = new MediaPipe::FLVTag();
	MediaPipe::FLVAudioTag* audio = new MediaPipe::FLVAudioTag(1 << 16);
}
