#ifndef _HTTP_H
#define _HTTP_H

struct http;

struct http *http_create();
int http_free(struct http *self);

int http_set_host(struct http *self, const char *host_name);

const char *http_get_cookie(struct http *self, const char *key);

int http_send_packet(struct http *self, const char *header, const char *body, int body_len, char *res, int len);

int http_send_packet_on(struct http *self, const char *header, const char *body, int body_len);
int http_recv_packet_off(struct http *self, char *res, int res_len);

#endif // !_HTTP_H
