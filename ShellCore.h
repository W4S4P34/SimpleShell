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
#define TOKENS_DELIM " \t\r\n\a"

#define clear() printf("\033[H\033[J")

// Prototypes
int init_greeting(void);
char* read_cmdline(void);
int check_pipe_cmd(char*);
char** parse_cmdline(char*);
char** parse_pipe_cmdline(char*);

#endif