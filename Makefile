SRC = array.c cookie.c http.c main.c socket.c
CC = gcc
CC_FLAG = -c -g

main: $(subst .c,.o,$(SRC))
	$(CC) -o $@ $^

main.o:main.c
array.o:array.c
cookie.o:cookie.c
http.o:http.c
socket.o:socket.c

%.o:%.c
	$(CC) $(CC_FLAG) -o $@ $<

clean:
	rm *.o
	rm main
