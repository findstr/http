/**
=========================================================================
 Author: findstr
 Email: findstr@sina.com
 File Name: array.c
 Description: (C)  2014-04  findstr
   
 Edit History: 
   2014-04-27    File created.
=========================================================================
**/
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "array.h"
struct array {
        int     em_size;
        int     em_cnt;
        int     buff_size;
        void    *buff;
};

struct array *array_create(int em_size)
{
        struct array *p = (struct array *)malloc(sizeof(struct array));
        if (p == NULL)
                return NULL;

        assert(em_size > 0);
        memset(p, 0, sizeof(struct array));

        p->em_size = em_size;
        
        return p;
}

int array_release(struct array *arr)
{
        assert(arr);
        if (arr->buff)
                free(arr->buff);
        
        free(arr);

        return 0;
}
int array_reserve(struct array *arr, int em_cnt)
{
        assert(arr);

        if (arr->em_cnt > em_cnt)
                return 0;

        arr->buff_size = em_cnt * arr->em_size;
        arr->buff = realloc(arr->buff, arr->buff_size);

        return 0;
}
int array_get(struct array *arr, int index, void *em)
{
        assert(arr);
        assert(em);
        assert(index < arr->em_cnt);

        memcpy(em, (unsigned char *)arr->buff + index * arr->em_size, arr->em_size);

        return 0;
}

void *array_ptr(struct array *arr, int em_index)
{
        assert(arr);
        assert(em_index < arr->em_cnt);

        return (unsigned char *)arr->buff + em_index * arr->em_size;
}

int array_push(struct array *arr, const void *em)
{
        int new_size;

        assert(arr);
        assert(em);

        new_size = arr->em_size * (arr->em_cnt + 1);
        if (new_size > arr->buff_size) {
                arr->buff_size = new_size * 2;
                arr->buff = realloc(arr->buff, arr->buff_size);
        }

        if (arr->buff == NULL)
                return -1;

        memcpy((unsigned char *)arr->buff + arr->em_cnt * arr->em_size, em, arr->em_size);
        arr->em_cnt++;

        //printf("--buff_size:%d, elm_cnt:%d\n", arr->buff_size, arr->em_cnt);

        return 0;
}

int array_erase(struct array *arr, int em_index)
{
        assert(arr);
        assert(em_index < arr->em_cnt);

        memcpy((unsigned char *)arr->buff + em_index * arr->em_size, (unsigned char *)arr->buff + (em_index + 1) * arr->em_size, arr->em_size * (arr->em_cnt - em_index - 1));
        arr->em_cnt--;

        return 0;
}

int array_size(struct array *arr)
{

        if (arr == NULL)
                return 0;

        assert(arr);

        return arr->em_cnt;
}

int array_clear(struct array *arr)
{
        assert(arr);
        arr->em_cnt = 0;

        return 0;
}
