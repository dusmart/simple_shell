#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LINE 80
#define TRUE 1
#define FALSE 0
#define HISTORY_NUM 10
// color
#define NONE          "\033[m"
#define COLOE_LIGHT_BLUE    "\033[1;34m"
#define COLOR_LIGHT_CYAN    "\033[1;36m"
#define COLOR_PURPLE        "\033[0;35m"


char history[HISTORY_NUM][MAX_LINE];
int history_index = 0;
char buf[1];
int fds[2];

int setup(char inputBuffer[], char *args[], int *background){
    int i, start = -1, count = 0, length = strlen(inputBuffer);
    char tmp[MAX_LINE];
    *background = FALSE;
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
        return 0;
    }
    else return -1;
}

void execute(char *args[], const int background){
    // fork a child process to do the command
    pid_t pid = fork();
    if(pid < 0){
        // handle error
        fprintf(stderr, COLOR_PURPLE"Fork failed!\n"NONE);
        exit(-1);
    }
    else if(pid == 0){
        // child process
        if(execvp(args[0], args) == -1){
            printf(COLOR_PURPLE"Warning: There is no such instruction.\n"NONE);
            memcpy(buf, "N", 1);
            write(fds[1], buf, 1);
        }
        exit(0);
    }
    else{
        //parent process
        if(background == FALSE){
            waitpid(pid, NULL, 0);
        }
        else sleep(0.2);
        int status = read(fds[0], buf, 1);
        if(buf[0]=='N' && status>0) history_index--;
    }
}

void handle_SIGINT(int sig){
    char inputBuffer[MAX_LINE];
    char *args[MAX_LINE/2 + 1];
    int length, background;
    int i = (history_index<10)?0:(history_index-10);
    printf("\n");
    for(;i<history_index;++i){
        printf("%d: ", i+1);
        printf("%s", history[i%10]);
        fflush(stdout);
    }
    //execute history command
    printf(COLOR_LIGHT_CYAN"HISTORY >> "NONE);
    fflush(stdout);
    if((length = read(STDIN_FILENO, inputBuffer, MAX_LINE)) == 0) exit(0);
    while(inputBuffer[0] != 'r' && (length==2 || length==4)){
        printf(COLOR_PURPLE"Warning: You can only use 'r x' or 'r' format to call history commands.'x' is the first letter of that command.\n"NONE);
        printf(COLOR_LIGHT_CYAN"HISTORY >> "NONE);
        fflush(stdout);
        if((length = read(STDIN_FILENO, inputBuffer, MAX_LINE)) == 0) exit(0);
    }
    if(length == 2){
        printf(COLOE_LIGHT_BLUE"old command is "NONE);
        printf("%s", history[(history_index-1)%10]);
        strcpy(inputBuffer, history[(history_index-1)%10]);
        setup(inputBuffer, args, &background);
        execute(args, background);
    }
    else if(inputBuffer[1] == ' '){
        for(i=history_index-1;i>=0&&i>=history_index-10;--i){
            if(history[i%10][0] == inputBuffer[2]) break;
        }
        printf(COLOE_LIGHT_BLUE"old command is "NONE);
        printf("%s", history[i%10]);
        strcpy(inputBuffer, history[i%10]);
        setup(inputBuffer, args, &background);
        execute(args, background);
    }
    fflush(stdout);
}

int main(){
    // initialize variables
    char inputBuffer[MAX_LINE];
    int length, background;
    char *args[MAX_LINE/2 + 1];
    // set up the signal handler
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    sigaction(SIGINT, &handler, NULL);
    pipe(fds);
    fcntl( fds[1], F_SETFL, fcntl(fds[1], F_GETFL) | O_NONBLOCK);
    fcntl( fds[0], F_SETFL, fcntl(fds[0], F_GETFL) | O_NONBLOCK);

    // the main loop
    while(TRUE){
        printf(COLOE_LIGHT_BLUE"COMMAND >> "NONE);
        fflush(stdout);
        // read command
        if((length = read(STDIN_FILENO, inputBuffer, MAX_LINE)) == 0) exit(0);
        inputBuffer[length] = '\0';
        // parse the args in command and store right command to history
        setup(inputBuffer, args, &background);
        execute(args, background);
    }
    return 0;
}
