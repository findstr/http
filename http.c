#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "cookie.h"
#include "socket.h"
#include "http.h"

#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))

struct http {
        char            host_name[128];
        struct cookie   *cook;
        struct socket   *sock;
};



struct http *http_create()
{
        struct http *h = (struct http *)malloc(sizeof(*h));
        if (h == NULL)
                return h;

        memset(h, 0, sizeof(*h));

        h->cook = cookie_create();
        h->sock = socket_create();
        
        if (h->cook && h->sock)
                return h;
        
        if (h->cook)
                cookie_free(h->cook);

        if (h->sock)
                socket_free(h->sock);

        free(h);


        return NULL;
}

int http_free(struct http *self)
{
        assert(self);

        cookie_free(self->cook);
        socket_free(self->sock);

        free(self);

        return 0;
}

int http_set_host(struct http *self, const char *host_name)
{
        assert(self);
        assert(host_name);

        strncpy(self->host_name, host_name, ARRAY_SIZE(self->host_name) - 1);

        return 0;
}

const char *http_get_cookie(struct http *self, const char *key)
{
        assert(self);
        assert(self->cook);
        assert(key);

        return cookie_get(self->cook, key);
}

static int send_header(struct http *self, const char *header)
{
        int err;
        char *url;
        int url_len;
        char cookie[2048];

        err = cookie_format(self->cook, cookie, ARRAY_SIZE(cookie));
        if (err < 0)
                return -1;

        url_len = strlen(header) + strlen(cookie) + 128; /* other charactor */

        url = (char *)alloca(url_len + sizeof(url[0]));
        if (url == NULL)
                return -1;

        if (err > 0)
                url_len = snprintf(url, url_len, "%sCookie:%s\r\n\r\n", header, cookie);
        else
                url_len = snprintf(url, url_len, "%s\r\n\r\n", header);

        err = socket_send_data(self->sock, url, url_len);

        return err;
}

static int send_body(struct http *self, const char *body, int body_len)
{
        int err;

        assert(self);
        assert(self->sock);

        err = 0;

        if (body)
                err = socket_send_data(self->sock, body, body_len);

        return err;
}

static int recv_header(struct http *self, int *data_len)
{
        int err;
        char url[8192];
        int  url_status;
        int  transfer_chunked;
        char *tmp;

        assert(self);
        assert(data_len);

        //Resonse
        err = socket_recv_line(self->sock, url, ARRAY_SIZE(url));
        if (err < 0)
                return err;

        if (strstr(url, "OK") == NULL)
                url_status = -1;
        else
                url_status = 0;

        //header
        transfer_chunked = 0;
        *data_len = 0;
        do {
                err = socket_recv_line(self->sock, url, ARRAY_SIZE(url));
                if (err < 0)
                        return err;

                tmp = strstr(url, "Content-Length: ");
                if (tmp)
                        sscanf(tmp + sizeof("Content-Length:"), "%d", data_len);
 
                tmp = strstr(url, "Transfer-Encoding: chunked");
                if (tmp)
                        transfer_chunked = 1;

                //Set-Cookie: PHPSESSID=3e406q4q311tgv4vhdom6ihbs6; path=/
                tmp = strstr(url, "Set-Cookie: ");
                if (tmp)
                        err = cookie_set(self->cook, tmp + sizeof("Set-Cookie"));

                if (err < 0)
                        return err;

        } while (err != 2 && url[0] != '\r' &&  url[1] != '\n');

        if (url_status)
                return -1;

        if (err < 0)
                return err;

        if (transfer_chunked && *data_len == 0)
                err = socket_recv_line(self->sock, url, ARRAY_SIZE(url));

        if (err < 0)
                return err;

        sscanf(url, "%x", data_len);

        return 0;
}

/*
static int utf8_to_mbs(char *pmb, int mb_len, const char* utf8, int utf8_len)  
{  
    // convert an UTF8 string to widechar   
        wchar_t *unicode;
        int len;
 
        len = MultiByteToWideChar(CP_UTF8, 0, utf8, utf8_len, NULL, 0);  
        
        unicode = (wchar_t *)alloca((len + 1) * sizeof(wchar_t));
        if (unicode == NULL)
                return -1;
  
        len = MultiByteToWideChar(CP_UTF8, 0, utf8, utf8_len, unicode, len);  
  
        // convert an widechar string to Multibyte   
        len = WideCharToMultiByte(CP_ACP, 0, unicode, len, pmb, mb_len, NULL, NULL);  
        if (len <= 0)
                return -1;

        return 0;
}
*/

int http_send_packet_on(struct http *self, const char *header, const char *body, int body_len)
{
        int err;
        int res_len;

        assert(header);

        res_len = 0;

        err = socket_connect_host(self->sock, self->host_name, 80);
        if (err < 0)
                return err;
 
        err = send_header(self, header);
        if (err < 0)
                goto end;

        err = send_body(self, body, body_len);
        if (err < 0)
                goto end;

        err = recv_header(self, &res_len);

end:
        if (err < 0) {
                socket_disconnect(self->sock);
                return err;
        }

        if (res_len == 0)
                socket_disconnect(self->sock);

        return res_len;
}


int http_recv_packet_off(struct http *self, char *res, int res_len)
{
        int err;

        if (res_len == 0)
                return 0;

        assert(res);
        err = socket_recv_data(self->sock, res, res_len);
        
        socket_disconnect(self->sock);

        return err;
}

int http_send_packet(struct http *self, const char *header, const char *body, int body_len, char *response, int len)
{
        int err;
        int data_len;
        char *res;

        assert(self);
        assert(header);

        err = http_send_packet_on(self, header, body, body_len);
        if (err < 0)
                return err;

        /* have no body */
        if (err == 0)
                return err;

        data_len = err;
        res = (char *)malloc(data_len);
        if (res == NULL) {
                err = http_recv_packet_off(self, (char *)&err, 1);      /* close the socket */
                return -1;
        }

        err = http_recv_packet_off(self, res, data_len);
        if (err < 0)
                goto end;

        if (response)
                strncpy(response, res, len);

end:
        if (res)
                free(res);

        return data_len;
}

void http_cookie_dump(struct http *self)
{
        char buff[8192];

        cookie_format(self->cook, buff, 8192);

        printf("cookie dump -->\n %s \n", buff);

        return ;
}
