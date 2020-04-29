#ifndef SHELLCORE_H
#define SHELLCORE_H
// Library
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

// Macro
#define TOKENS_BUFFER_SIZE 64
#define TOKENS_DELIM " \t\r\n\a"
#define TOKEN_PIPE_DELIM "|"

#define MAX_BUFFER_CWD 1024

#define BUILTIN_LIST_SIZE 4

#define HISTORY_LIST_SIZE 10

#define clear() printf("\033[H\033[J")

// Prototypes
int initGreeting(void);
char* readCmdLine(void);
int checkPipeCmd(char*);
char** parseCmdLine(char*, char*);
int getCmdType(char*, char**);
int executeBuiltinCmdLine(int, char**, char**);
int executeBinCmdLine(char**);
int executePipeCmdLine(char**, char**);

// Built-in features
int executeHelpCmd();
int executeHistoryCmd(char**, char**);
int executeChangeDirCmd(char**);
int executeExitCmd();

// Sub features
int addHistory(char*, char**, int*);
int xulychamthan(char*);

#endif