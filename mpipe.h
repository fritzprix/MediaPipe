/*
 * mpipe.h
 *
 *  Created on: Mar 17, 2016
 *      Author: innocentevil
 */

#ifndef MPIPE_H_
#define MPIPE_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifndef offsetof
#define offsetof(type,member)					(size_t)(&((type*) 0)->member)
#endif


#ifndef container_of
#define container_of(ptr,type,member) 		(type*) (((size_t) ptr - (size_t) offsetof(type,member)))
#endif


#ifdef __cplusplus
}
#endif


#endif /* MPIPE_H_ */
