
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "shell.h"

#define PORT 9000

static void die(const char* msg) {
	fputs(msg, stderr);
	putc('\n', stderr);
	exit(-1);
}

char* cdCommand(char * newDirPath, Loop * loop, int cfd) {
    if (newDirPath == NULL || chdir(newDirPath) < 0) {
		return "Couldn't change directory: not found.\n";
    } else {
        changeDir(newDirPath, loop);
    }
	return "Directory changed.\n";
}

char* runProg(char** args, Loop* loop) {
    pid_t pid;
    pid = fork();
    char* result;
		
    if (pid == 0) { //child     
        int childStatus = execvp(args[0], args);
        if (childStatus == -1) {
            result = "Couldn't executing program.\n";
        }
        exit(0);
    } else if (pid > 0) { //parent
		sprintf(result, "%ld\n", (long) pid);
    } else {
        result = "Couldn't fork the process.\n";
    }
    return result;
}

char* handleInput(char** input, Loop* loop, int cfd) {
	if (strcmp(input[0], "cd") == 0) {
		return cdCommand(input[1], loop, cfd);
	} else if (strcmp(input[0], "wait") == 0) {
		return "___wait\n";
	} else {
		return runProg(input, loop);
	}
	return "";
}

int main() {
	struct sockaddr_in srv_addr, cli_addr;
	socklen_t sad_sz = sizeof(struct sockaddr_in);
	int sfd, cfd;
	ssize_t bytes;
	pid_t pid;
	
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(PORT);
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		die("Couldn't open the socket");
	
	if (bind(sfd, (struct sockaddr*) &srv_addr, sad_sz) < 0)
		die("Couldn't bind socket");
	
	if (listen(sfd, 1) < 0)
		die("Couldn't listen to the socket");
	
	while (1) {
		cfd = accept(sfd, (struct sockaddr*) &cli_addr, &sad_sz);
		if (cfd < 0)
			die("Couldn't accept incoming connection");
			
		pid = fork();
		if (pid < 0)
			die("Couldn't fork process.");
		else if (pid > 0) {
			// close(cfd);
		}
		else {
			
			// dup2(cfd, STDOUT_FILENO);
			
			char* text;
			char buf[256];
			char** parsedInput;
			Loop* loop = initLoop();
			// // int inputSuccess;
			
			text = concat(buildCurrentDir(loop->entryDir), ">");
			if (write(cfd, text, strlen(text)) < 0)
				die("Couldn't send message");
			
			while (loop->status == 0) {
				
				while ((bytes = read(cfd, buf, strlen(buf))) != 0)
				{
					if (bytes < 0)
						die("Couldn't receive message");
					
					parsedInput = parseInput(buf);
					
					text = handleInput(parsedInput, loop, cfd);
				
					text = concat(text, buildCurrentDir(loop->entryDir));
					text = concat(text, ">");
					// text = concat(text, parsedInput[0]);
					
					// printf("\n%li:\n%s\n", strlen(text), text);
					
					if (write(cfd, text, strlen(text)) < 0)
						die("Couldn't send message");
				}
				
				// bytes = read(cfd, buf, sizeof(buf));
				// if (bytes < 0)
					// die("Couldn't receive message");
				// // input = readInput();
				// printf("\n---\n%s\n---\n", buf);
				// //inputSuccess = handleInput(input, loop);
				// // if (inputSuccess == 1) {
					// // Todo: do something
				// // }
			}
				
			// while ((bytes = read(cfd, buf, sizeof(buf))) != 0)
			// {
				// if (bytes < 0)
					// die("Couldn't receive message");
				
				// if (write(cfd, buf, bytes) < 0)
					// die("Couldn't send message");
			// }
			
			// close(sfd);
	
		}
	}
	
	close(cfd);
	close(sfd);
	return 0;
}
