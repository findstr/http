#ifndef _COOKIE_H
#define _COOKIE_H

struct cookie;

struct cookie *cookie_create();
int cookie_free(struct cookie *self);

int cookie_set(struct cookie *self, const char *cookie);
const char *cookie_get(struct cookie *self, const char *cookie);

int cookie_format(struct cookie *self, char *cookie, int len);  /* cookie max size is 2048 */



#endif