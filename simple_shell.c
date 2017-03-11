#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 80
#define TRUE 1
#define FALSE 0
#define HISTORY_NUM 10


char history[HISTORY_NUM][MAX_LINE];
int history_index = 0;

void setup(char inputBuffer[], int length, char *args[], int *background){
    int i, start = -1, count = 0;
    char tmp[MAX_LINE];
    memcpy(tmp, inputBuffer, MAX_LINE);
    for(i=0; i<length; ++i){
        switch(inputBuffer[i]){
            case ' ':
            case '\t':
                if(start != -1){
                    args[count] = &inputBuffer[start];
                    count++;
                }
                inputBuffer[i] = '\0';

                start = -1;
                break;
            case '\n':
                if(start != -1){
                    args[count] = &inputBuffer[start];
                    count++;
                }
                inputBuffer[i] = '\0';
                args[count] = NULL;
                break;
            default:
                if(start == -1) start = i;
                if(inputBuffer[i] == '&'){
                    *background = TRUE;
                    inputBuffer[i] = '\0';
                    start = -1;
                }
        }
    }
    args[count] = NULL;
    if(count != 0){
        memcpy(history[(history_index++)%10], tmp, MAX_LINE);
        //printf("\n---history number = %d---\n",history_index);
    }
}

void handle_SIGINT(int sig){
    int i = (history_index<10)?0:(history_index-10);
    printf("\n");
    for(;i<history_index;++i){
        printf("%d: ", i);
        printf("%s", history[i%10]);
        fflush(stdout);
    }
    //exec_history_command();
    char inputBuffer[MAX_LINE];
    if((length = read(STDIN_FILENO, inputBuffer, MAX_LINE)) == 0) exit(0);


}

int main(){
    // initialize variables
    char inputBuffer[MAX_LINE];
    int length, background;
    char *args[MAX_LINE/2 + 1];
    pid_t pid;
    // set up the signal handler
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    sigaction(SIGINT, &handler, NULL);

    // the main loop
    while(TRUE){
        background = FALSE;
        printf("COMMAND >> ");
        fflush(stdout);
        // read command
        if((length = read(STDIN_FILENO, inputBuffer, MAX_LINE)) == 0) exit(0);
        inputBuffer[length] = '\0';
        // parse the args in command and store right command to history
        setup(inputBuffer, length, args, &background);
        // fork a child process to do the command
        pid = fork();
        if(pid < 0){
            // handle error
            fprintf(stderr, "Fork failed!\n");
            exit(-1);
        }
        else if(pid == 0){
            // child process
            execvp(args[0], args);
            exit(0);
        }
        else{
            //parent process
            if(background == FALSE){
                waitpid(pid, NULL, 0);
            }
        }

    }
}
