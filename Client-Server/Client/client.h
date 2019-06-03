
#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static void die(const char* msg) {
	fputs(msg, stderr);
	putc('\n', stderr);
	exit(-1);
}

#endif
