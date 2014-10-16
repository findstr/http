/**
=========================================================================
 Author: findstr
 Email: findstr@sina.com
 File Name: array.h
 Description: (C)  2014-04  findstr
   
 Edit History: 
   2014-04-27    File created.
=========================================================================
**/
#ifndef _ARRAY_H
#define _ARRAY_H

#ifdef __cplusplus

extern "C" {

#endif


#define	array_at(arr, index, type)	(((type *)array_ptr(arr, index))[0])

struct array *array_create(int element_size);
int array_reserve(struct array *arr, int em_cnt);
int array_release(struct array *array);

int array_size(struct array *arr);
int array_clear(struct array *arr);

void *array_ptr(struct array *arr, int em_index);

int array_push(struct array *arr, const void *em);
int array_get(struct array *arr, int em_index, void *em);
int array_erase(struct array *arr, int em_index);

#ifdef __cplusplus
}
#endif

#endif
