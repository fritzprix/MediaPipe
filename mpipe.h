/*
 * mpipe.h
 *
 *  Created on: Mar 17, 2016
 *      Author: innocentevil
 */

#ifndef MPIPE_H_
#define MPIPE_H_



namespace MediaPipe {

template <class T>
class Iterator {
public:
	Iterator(){};
	virtual ~Iterator() {};
	virtual bool hasNext() = 0;
	virtual T* next() = 0;
	virtual void remove() = 0;
};

template <class T>
class Iterable {
public:
	Iterable(){};
	virtual ~Iterable() {};
	virtual Iterator<T>* iterator() = 0;
};


}


#endif /* MPIPE_H_ */
