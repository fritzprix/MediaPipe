/*
 * test.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: innocentevil
 */

#include "core/stream/MediaStreamTestUnit.h"

using namespace MediaPipe;

int main(){
	MediaStreamTestUnit* mstr_test = new MediaStreamTestUnit();
	mstr_test->performTest();
}
