
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#define PORT 9000
#define HOST "127.0.0.1"

static void die(const char* msg) {
	fputs(msg, stderr);
	putc('\n', stderr);
	exit(-1);
}

int main() {
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr.s_addr = inet_addr(HOST)
	};
	char buf[256];
	int readBytes;
	int cfd;
	char input[256];
	
	if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		die("Couldn't open the socket");
	
	if (connect(cfd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
		die("Couldn't connect to socket");
	
	int loop = 1;
	while (loop > 0) {
		
		if ((readBytes = read(cfd, buf, sizeof(buf))) < 0)
			die("Couldn't receive message");
		
		buf[readBytes] = 0;
		
		printf("[recv:%li] %s", strlen(buf), buf);
		
		// for(int i = 0; i < strlen(buf); i++) {
			// printf("\n[%i] %c", i, buf[i]);
		// }
		
		if (fgets(input, 256, stdin) == NULL) {
			die("Couldn't get user input.");
		}
		
		if (strncmp(input, "exit", 4) == 0) {
			loop = 0;
		} else if (write(cfd, input, strlen(input)) < 0) {
			die("Couldn't send message");
		}
	}
	
	close(cfd);
	return 0;
}
