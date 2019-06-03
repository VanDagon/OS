#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct Dir {
    char* dirName;
    struct Dir* nextDir;
} Dir;

typedef struct Loop {
    Dir* entryDir;
    int status;
} Loop;

int waitStatus = 0;

Loop* initLoop(){
    Loop* loop = (Loop*)malloc(sizeof(Loop));
    loop->status = 0;
    loop->entryDir = (Dir*)malloc(sizeof(Dir));
    if (loop->entryDir == NULL) {
        printf("initLoop: memory allocation failed for entryDir\n"); //Todo: debug-message
    }
    loop->entryDir->dirName = ".";
    return loop;
}

char* concat(char* string1, char* string2) {
    int l1 = strlen(string1);
    int l2 = strlen(string2);
    char* result = malloc(l1+l2+1); // Maybe use realloc(result,l1+l2+1) instead?
    if (result==NULL){printf("Concat: memory allocation failed\n");} //Todo: check for errors in malloc here
    strcpy(result, string1);
    strcat(result, string2);
    return result;
}

char* appendChar(char* str, char c) {
    int l1 = strlen(str);
    char * result = malloc(l1+2);
    strcpy(result, str);
    result[l1] = c;
    result[l1+1] = '\0';
    return result;
}

char* buildCurrentDir(Dir* startDir) {
    char* dirs = "";
    if (startDir != NULL) {
        dirs = startDir->dirName;
        dirs = concat(dirs, "/");
        Dir* tmp = startDir->nextDir;
        while(tmp != NULL) {
            dirs = concat(dirs, tmp->dirName);

            tmp = tmp->nextDir;

            dirs = concat(dirs, "/");
        }
    }
    return dirs;
}

int runProg2(Loop* loop, char** args) {
    pid_t pid;
    pid = fork();
        
    if (pid == 0) { //child     
        int childStatus = execvp(args[0], args);
        if (childStatus == -1) {
            printf("failed execv %s", args[0]);
            //Todo: do something
        }
        exit(0);
    } else if (pid > 0) { //parent
        printf("%ld\n", (long) pid);
    } else {
        //Todo: do something, failure
    }
    return 0;
}

int runWithPipe(Loop* loop, char** args1, char** args2) {
    int fd[2];
    pid_t pid1;
    //pid_t pid2;
    
    int pipeStatus = pipe(fd);
    if (pipeStatus < 0) {
        //Todo: do something
    }
    
    pid1 = fork();
    if (pid1 == 0) { //child
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        
        int childStatus = execvp(args1[0], args1);
        if (childStatus -1) {
            printf("failed execv %s", args1[0]);
            //Todo: do something
        }
    }
    return 0;
}

void exitWait(int sig) {
    if (sig != SIGINT) {
        return;
    }
    waitStatus = 1;
}

int waitFor(long* pids) { // has to be size of 8 elements
    int status;
    pid_t pid = wait(&status);
    while (pid > 0 && waitStatus == 0) {
        for (int i = 0; i < 8; i++) {
            if (pid == pids[i]) {
                printf("%ld exited with status %i", (long) pid, status);
            }
        }
        pid = wait(&status);
    }
    //Todo: error handling
    waitStatus = 0;
    return 0;
}

void changeDir(char * newDir, Loop * loop) {
    char* tmp = "";
    Dir* tmpDir = loop->entryDir;
    while (tmpDir->nextDir != NULL) {
        tmpDir = tmpDir->nextDir;
    }
    while (newDir[0]!='\0')
    {
        if (newDir[0]=='/')
        {
                tmpDir->nextDir = (Dir*)malloc(sizeof(Dir));
                if (tmpDir->nextDir == NULL) {
                    printf("initLoop: memory allocation failed for entryDir\n");
					return;
                }
                tmpDir->nextDir->dirName = tmp;
                tmpDir = tmpDir->nextDir;
                tmp = "";
        }
        else
        {
            tmp = appendChar(tmp,newDir[0]);
        }
        newDir++;
    }
    if (strcmp(tmp, "") != 0) {
        tmpDir->nextDir = (Dir*)malloc(sizeof(Dir));
        if (tmpDir->nextDir == NULL) {
            printf("initLoop: memory allocation failed for entryDir\n");
			return;
        }
        tmpDir->nextDir->dirName = tmp;
    }
}

int cdCommand2(char * newDirPath, Loop * loop) {
    // newDirPath += 3; // slide pointer to where the path begins in input
    if (newDirPath == NULL || chdir(newDirPath) < 0)
    {
        printf("cd: directory not specified"); 
        return 1; 
    } else {
        changeDir(newDirPath,loop); 
    }
    return 0;
}

char* readInput(void) {
	int max = 20;
    char* name = (char*) malloc(max); // allocate buffer
    // if (name == 0) quit();

    printf("Enter a file name: ");

    while (1) { // skip leading whitespace
        int c = getchar();
        if (c == EOF) break; // end of file
        if (!isspace(c)) {
             ungetc(c, stdin);
             break;
        }
    }

    int i = 0;
    while (1) {
        int c = getchar();
        if (isspace(c) || c == EOF) { // at end, add terminating zero
            name[i] = 0;
            break;
        }
        name[i] = c;
        if (i == max - 1) { // buffer full
            max += max;
            name = (char*) realloc(name, max); // get a new and larger buffer
            // if (name == 0) quit();
        }
        i++;
    }

    printf("The filename is %s\n", name);
	return name;
}

char ** parseInput(char * input)
{
    int bufferIncrement = 10;
    char ** args; // args[0] is the name of program, args[1-...] are arguments
    char * temp = "";
    int counter = 0;
    int maxStrings = 21; // maximum amount of arguments (- 1, since args[0] is programName)
    args =  malloc(sizeof(char*)*maxStrings);
    // store text until the first whitespace (program name)   into temp
    while (input[0]!=' ' && input[0]!= '\0' && input[0] != '\n')
    {
        temp = appendChar(temp,input[0]);
        input++;

    }
    args[0] = (char*) malloc(sizeof(char) *strlen(temp));
    strcpy(args[0],temp);
    input++;

    strcpy(temp,"");
    // get following arguments
    while(1)
    {
        if (strlen(input)!=0 && input[0]!= ' ')
        {
            temp = appendChar(temp,input[0]);
        }
        else
        {

            if(counter++==maxStrings)
            {
                maxStrings+=bufferIncrement;
                args = realloc(args, sizeof(char*) * maxStrings); // allocate extra pointers in args for more arguments
            }

            args[counter] = (char*) malloc(sizeof(char) * strlen(temp));
            strcpy(args[counter],temp);

            strcpy(temp,"");
            if (strlen(input)==0){break;}
        }
        input++;
    }
    return args;
}

int handleInput2(char* input, Loop* loop) {
    char ** parsedInput = parseInput(input);

    if (input[0]==EOF) {printf("input is empty");return 0;}
    else if (strcmp(parsedInput[0],"exit")==0){
        loop->status = 1;
    }
    else if (strcmp(parsedInput[0], "cd") == 0){
        cdCommand2(parsedInput[1], loop); // pass first argument after command 'cd' as address
    } else if (strncmp(parsedInput[0], "wait",4)==0) {
        //Todo: pass parsed input
        char* ptr;
        long* pids = (long[]){strtol(strndup(input+5, 4), &ptr, 10)};
        waitFor(pids);
    } else { // cases where the input starts with ./[programName] or [programName]
        if (parsedInput[1]!=NULL)
        {
            runProg2(loop, parsedInput); 
        }
    }

    return 0;
}

int loop(void) {
	
    Loop* loop = initLoop();

    // char* input;
    // int inputSuccess;
    while (loop->status == 0) {
        printf("%s", buildCurrentDir(loop->entryDir));
        printf(">");
        // input = readInput();
		// printf("\n---\n%s\n---\n", input);
        //inputSuccess = handleInput(input, loop);
        // if (inputSuccess == 1) {
            // Todo: do something
        // }
    }

    return 0;
}

// int main(int argc, const char* argv[]) {

    // signal(SIGINT, exitWait);

    // loop();
// }
