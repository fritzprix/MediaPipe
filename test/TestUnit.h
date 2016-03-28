/*
 * TestUnit.h
 *
 *  Created on: Mar 29, 2016
 *      Author: innocentevil
 */

#ifndef TEST_TESTUNIT_H_
#define TEST_TESTUNIT_H_

namespace MediaPipe {

class TestUnit {
public:
	TestUnit() {};
	virtual ~TestUnit() {};
	virtual bool performTest() = 0;
};

} /* namespace MediaPipe */

#endif /* TEST_TESTUNIT_H_ */
