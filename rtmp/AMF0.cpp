/*
 * AMF0.cpp
 *
 *  Created on: Mar 19, 2016
 *      Author: innocentevil
 */

#include "AMF0.h"
#include "cdsl_dlist.h"

namespace MediaPipe {

AMF0::AMF0Object::AMF0Object() {
	cdsl_dlistInit(&list_node);
}

AMF0::AMF0Object::~AMF0Object() {
}

AMF0::AMF0() {

}

AMF0::~AMF0() {
}

void AMF0::add(AMF0Object* obj) {
}

int AMF0::lenght(void) {
}

AMF0::AMF0Object* AMF0::remove(int idx) {
}

void AMF0::remove(AMF0Object* obj) {
}

} /* namespace MediaPipe */

ssize_t MediaPipe::AMF0::read(uint8_t* data) {
}

ssize_t MediaPipe::AMF0::serialize(MediaContext* meta, MediaStream* stream) {
}

ssize_t MediaPipe::AMF0::deserialize(MediaContext* meta, MediaStream* stream) {
}
