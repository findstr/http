#include <assert.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

#include "socket.h"

#define	ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#pragma comment(lib, "ws2_32.lib")

struct socket {
	int     s;
};

struct socket *socket_create()
{
	struct socket *s;

	s = (struct socket *)malloc(sizeof(*s));

	if (s == NULL)
		return NULL;

	memset(s, 0, sizeof(*s));

	s->s = -1;

	return s;
}

int socket_free(struct socket *self)
{
	assert(self);

	if (self->s != -1)
		close(self->s);

	free(self);

	return 0;
}

int socket_connect_host(struct socket *self, const char *host_name, unsigned short dst_port)
{
        int err;
        struct hostent	*host;
        int              s;
        struct sockaddr_in addr;

	host = gethostbyname(host_name);
	if (host == NULL)
		return -1;

        addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = *(u_long *) host->h_addr_list[0];
	addr.sin_port = htons(dst_port);

        s = socket(AF_INET, SOCK_STREAM, 0);
        err = connect(s, (struct sockaddr *)&addr, sizeof(addr));

        if (err < 0)
                return 0;

        self->s = s;

        return 0;
}

int socket_connect(struct socket *self, const char *dst_ip, unsigned short dst_port)
{
	int err;
	int s;
	struct sockaddr_in sa;

	assert(self);
	assert(dst_ip);

	s = socket(AF_INET, SOCK_STREAM, 0);

	sa.sin_addr.s_addr = inet_addr(dst_ip);
	sa.sin_port = htons(dst_port);
	sa.sin_family = AF_INET;

	err = connect(s, (struct sockaddr *)&sa, sizeof(sa));
	if (err < 0)
		return -1;

	self->s = s;
	return 0;
}

int socket_disconnect(struct socket *self)
{
	assert(self->s != -1);

	close(self->s);

	self->s = -1;

	return 0;
}

int socket_send_data(struct socket *self, const char *buff, int len)
{
	int err;

	assert(self);
	assert(buff);
	assert(len);
	assert(self->s != -1);

	do {
		err = send(self->s, buff, len, 0);
		if (err < 0)
			break;

		len -= err;
		buff += err;
	} while (len);

	if (len == 0)
		return 0;
	else
		return -1;
}

int socket_recv_data(struct socket *self, char *buff, int len)
{
	int err;

	assert(self);
	assert(buff);
	assert(len);
	assert(self->s != -1);

	do {
		err = recv(self->s, buff, len, 0);
		if (err <= 0)
			break;

		len -= err;
		buff += err;
	} while (len);

	if (len == 0)
		return 0;
	else
		return -1;
}

int socket_send_str(struct socket *self, const char *buff)
{
	int err;
	int len;

	assert(self);
	assert(self->s != -1);
	assert(buff);

	len = 0;

        len = strlen(buff);

	err = socket_send_data(self, buff, len);

	return err;
}

int socket_recv_line(struct socket *self, char *buff, int len)
{
        int err;
        char *orig;

	assert(self);
	assert(self->s != -1);
	assert(buff);

        orig = buff;
	memset(buff, 0, len);

        do {
                err = recv(self->s, buff, 1, 0);
                if (err <= 0)
                        break;

                len--;
                buff++;
        } while (buff[-1] != '\n');

        if (err <= 0)
                return -1;
	else
		return buff - orig;
}
