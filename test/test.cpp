/*
 * test.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: innocentevil
 */

#include "MediaStreamTestUnit.h"
#include "FLVTestUnit.h"

using namespace MediaPipe;

int main(){
	MediaStreamTestUnit* mstr_test = new MediaStreamTestUnit();
	mstr_test->performTest();
	delete mstr_test;
	FLVTestUnit* flv_test = new FLVTestUnit();
	flv_test->performTest();
	delete flv_test;
}
