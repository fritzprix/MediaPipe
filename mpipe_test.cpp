/*
 * mpipe_test.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: innocentevil
 */

#include "FLVTag.h"


int main(void)
{
	FLVTag* flvTag = new FLVTag();
	FLVAudioTag* audio = new FLVAudioTag(flvTag, 1 << 16);
	audio->setSoundFormat(FLVAudioTag::AAC);
}
