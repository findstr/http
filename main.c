#include <stdio.h>

#include "http.h"

int main()
{
        const char *request =
                "GET http://www.baidu.com/ HTTP/1.1\r\n"
                "User-Agent: MySelf\r\n"
                "Host: www.baidu.com\r\n"
                "\r\n\r\n";


        char buff[8192];
        struct http *h;

        h = http_create();

        http_set_host(h, "www.baidu.com");

        http_send_packet(h, request, NULL, 0, buff, 8192);

        printf("response:%s\n", buff);

        http_cookie_dump(h);

        printf("cookie get:%s\n", http_get_cookie(h, "BAIDUID="));

        http_free(h);

        return 0;
}
