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
#define TOKENS_BUFFER_SIZE 32
#define TOKENS_DELIM " \t\r\n\a"

#define clear() printf("\033[H\033[J")

// Prototypes
int initGreeting(void);
char* readCmdLine(void);
int checkPipeCmd(char*);
char** parseCmdLine(char*);
char** parsePipeCmdLine(char*);
int executeCmdLine(char**);
int executePipeCmdLine(char**);

#endif