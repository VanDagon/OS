#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define INPUT_SIZE 256
#define RECEIVED_MESSAGE_SIZE 256

static inline void die(const char* msg)
{
	perror(msg);
	exit(-1);
}

int main()
{
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(8000),
		.sin_addr.s_addr = inet_addr("127.0.0.1")
	};
	char input[INPUT_SIZE];
	char received_messsage[256]
	int cfd;
	
	if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		die("Couldn't open the socket");
	
	if (connect(cfd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
		die("Couldn't connect to socket");
	
	while (1)
	{
		memset(input,0,strlen(input)); // clear input variable
		scanf("%s",input);

		(write(cfd, input, strlen(input)) < 0)
		{
			die("Could not send message.")
		}

		printf("[client]: %s\n",input);

		if (read(cfd, buf, sizeof(buf)) < 0)
			die("Could not receive message");
		
		printf("[server]: %s\n",buf);
	}
	
	close(cfd);
	return 0;
}
