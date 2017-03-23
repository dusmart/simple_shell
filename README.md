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
4. execute history command in **HISTORY** mode when user enter 'r x', 'x' is the first letter of that command

## Outline

```
char history[HISTORY_NUM][MAX_LINE];
int history_index = 0;
char buf[1];
int fds[2];

int setup(char inputBuffer[], char *args[], int *background);
void execute(char *args[], const int background);
void handle_SIGINT(int sig);
int main();
```

1. ```char history[10][80]``` stores all infomation of last 10 commands entered by user
2. ```int history_index``` indicate which history is the last command
3. ```char buf[1];int fds[2];``` provide some space for pipe communication between parent and child 
4. ```setup()``` take inputBuffer's command, separating it into distinct tokens using white space as delimiters. ```setup()``` modifies the args parameters so that it holds pointers to every tokens in inputBuffer. 
5. ```execute()``` take parsed command, executing it in child process and deciding whether or not suspend the main process.Plus, if the command cannot be executed we will decrease the history_index so that the wrong command will not be in history

### main function

1. initialize variables
2. set up the signal handler
3. enter the main loop: read command and call setup() and execute()

### execute function

1. fork a child process
2. different procedure for child and parent

### handle_SIGINT function

1. output most recent 10 history
2. read command from **HISTORY** mode
3. different procedure for 'r x' and 'r' command, this part is the same as main function

### setup function

This part is provided in the book(OPERATING SYSTEM CONCEPTS --7th edition -- Page122)

I change it to support history feature. That is to say, when a command is parsed successfully, I add it to the history and update the history_index.

## source code and demo

[https://github.com/dusmart/simple_shell](https://github.com/dusmart/simple_shell)

![img](https://dusmart.github.io/assets/img/2017-03-11-1.png)