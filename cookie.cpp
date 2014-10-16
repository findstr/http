#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "array.h"
#include "cookie.h"

struct cookie {
        struct array *tbl;
};

static int strcpy_end(char *dst, int len, const char *src, char end)
{
        assert(dst);
        assert(src);

        while (*src != end && len-- && *src != 0 && *src != '\r' && *src != '\n')
                *dst++ = *src++;

        if (len == 0)
                dst[-1] = 0;
        else
                *dst= 0;

        return 0;
}

static int strcmp_end(char *dst, const char *src, char end)
{
        assert(dst);
        assert(src);

        while (*src != end && *dst != end && *src != 0 && *dst != 0) {
                if (*dst != *src)
                        break;

                dst++;
                src++;
        }

        if (*dst == *src)
                return 0;
        else
                return 1;
}


struct cookie *cookie_create()
{
        struct cookie *c = (struct cookie *)malloc(sizeof(*c));
        if (c == NULL)
                return c;

        memset(c , 0, sizeof(*c));

        c->tbl = array_create(sizeof(char *));
        if (c->tbl == NULL) {
                free(c);
                return NULL;
        }

        array_reserve(c->tbl, 8);

        return c;
}

int cookie_free(struct cookie *self)
{
        int i;
        assert(self);

        for(i = 0; i < array_size(self->tbl); i++) {
                assert(array_at(self->tbl, i, char *));
                free(array_at(self->tbl, i, char *));
        }

        array_release(self->tbl);

        return 0;
}

int cookie_set(struct cookie *self, const char *cookie)
{
        int i;
        int err;
        int cookie_len;
        char *cookie_buff;

        assert(self);
        assert(self->tbl);
        err = 0;
        
        cookie_len = strlen(cookie) + 1;
        cookie_buff = (char *)malloc(cookie_len);
        if (cookie_buff == NULL)
                return -1;

        strcpy_end(cookie_buff, cookie_len, cookie, ';');

        for (i = 0; i < array_size(self->tbl); i++) {
                if (strcmp_end(array_at(self->tbl, i, char *), cookie, ';') == 0)
                        break;
        }

        if (i >= array_size(self->tbl)) {
                err = array_push(self->tbl, &cookie_buff);
        } else {
                free(array_at(self->tbl, i, char *));
                array_at(self->tbl, i, char *) = cookie_buff;
        }
        return err;
}

const char *cookie_get(struct cookie *self, const char *cookie)
{
        int i;

        assert(self);
        assert(self->tbl);
        assert(cookie);

        for (i = 0; i < array_size(self->tbl); i++) {
                if (strstr(array_at(self->tbl, i, char *), cookie))
                        return array_at(self->tbl, i, char *);
        }

        return NULL;
}

int cookie_format(struct cookie *self, char *cookie, int len)  /* cookie max size is 2048 */
{
        int i;
        int curr_len;
        int cookie_cnt;
        assert(self);
        assert(cookie);
        assert(len);

        curr_len = 0;
        cookie_cnt = array_size(self->tbl);

        for (i = 0; i < cookie_cnt - 1; i++)
                curr_len += _snprintf(&cookie[curr_len], len - curr_len, "%s;", array_at(self->tbl, i, char *));
 
        if (i < cookie_cnt)
                curr_len += _snprintf(&cookie[curr_len], len - curr_len, "%s", array_at(self->tbl, i, char *));

        return curr_len;
}