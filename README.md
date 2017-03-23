---
layout:     post
title:      "OS Project2 -- Simple Shell"
author:     "dusmart"
tags:
    - project
---

> A simple shell with history feature.

<!--more-->

---

## Feature

1. execute normal executable commands in **COMMAND** mode
2. execute command in background if a command end with ' &'
3. output most recent 10 commands and enter **HISTORY** mode when user enters \<Control\>\<C\>, wrong command will not be in history
4. execute history command in **HISTORY** mode when user enter 'r x' or 'r', 'x' is the first letter of that command,'r' stand for executing the most recent command

## Outline

```
char history[HISTORY_NUM+1][MAX_LINE];
int history_index = 0;
char buf[1];
int fds[2];

int setup(char inputBuffer[], char *args[], int *background);
void execute(char *args[], const int background);
void handle_SIGINT(int sig);
int main();
```

1. ```char history[HISTORY_NUM+1][MAX_LINE];``` stores all infomation of last 10 commands entered by user
2. ```int history_index``` indicate which history is the last command
3. ```char buf[1];int fds[2];``` provide some space for pipe communication between parent and child 
4. ```setup()``` take inputBuffer's command, separating it into distinct tokens using white space as delimiters. ```setup()``` modifies the args parameters so that it holds pointers to every tokens in inputBuffer. 
5. ```execute()``` take parsed command, executing it in child process and deciding whether or not suspend the main process.Plus, if the command cannot be executed we will decrease the history_index so that the wrong command will not be in history

### main function

1. initialize variables
2. set up the signal handler
3. set up non block pipe for communication between child and parent
4. the main loop: read command and call setup() and execute()

### execute function

1. fork a child process
2. different procedures for parent and child(execute command or tell parent the failure)

### handle_SIGINT function

1. output most recent 10 histories
2. read command from **HISTORY** mode
3. different procedure for 'r x' and 'r' command, this part is the same as main function

### setup function

This is provided in the book(OPERATING SYSTEM CONCEPTS --7th edition -- Page122)

I slightly change it to support history feature. That is to say, when a command is parsed successfully, I add it to the history and update the history_index.

## important points found by debugging

1. **communication** After we create the child by ```fork()```, we simply call exec() series function to execute the command. The execvp() will return -1 when it cannot execute the command. Instead of returning 0 when command is executed successfully, it won't return control to child. Thus, we must use non-blocking communication for child and parent. That's why we will ```sleep(0.2)``` to wait child after forking it in background mode. What's more,because we just need to know whether it is executed successfully or not, the message passing is better than memory sharing for communication. Here is a way to accomplish it.[link to stackoverflow](http://stackoverflow.com/questions/36673972/non-blocking-read-on-pipe)

2. **history capacity** Since we won't know if a command which passes parsing will be executed successfully as well, we simply add the cammand to history after it is parsed successfully. Yet when it is found illegal after calling exevcp(), we decrease the history index. Therefore, we must keep one more space for wrong command instead of just using 10. That why you see ```char history[HISTORY_NUM+1][MAX_LINE];``` instead of ```char history[HISTORY_NUM][MAX_LINE];```.


## source code and demo

[https://github.com/dusmart/simple_shell](https://github.com/dusmart/simple_shell)

![img](https://dusmart.github.io/assets/img/2017-03-11-1.png)