/*
 * Iterator.h
 *
 *  Created on: Apr 6, 2016
 *      Author: innocentevil
 */

#ifndef CORE_ITERATOR_H_
#define CORE_ITERATOR_H_

namespace MediaPipe {

template <class T>
class Iterator {
public:
	Iterator(){};
	virtual ~Iterator() {};
	virtual void reset() = 0;
	virtual bool hasNext() = 0;
	virtual T* next() = 0;
	virtual void remove() = 0;
	virtual bool lock() = 0;
	virtual void unlock() = 0;
};

template <class T>
class Iterable {
public:
	Iterable(){};
	virtual ~Iterable() {};
	virtual Iterator<T>* iterator() = 0;
};


}



#endif /* CORE_ITERATOR_H_ */
