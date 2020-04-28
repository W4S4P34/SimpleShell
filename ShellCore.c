#include "ShellCore.h"

int main(void)
{
    char* username = NULL, * full_cwd_dir = NULL, * cwd_dir;
    int cwd_buffer_coefficient;

    char* input_string = NULL;  
    
    char* builtin_cmd_list[BUILTIN_LIST_SIZE] = {"help", "history", "cd", "exit"};

    char* history_list[HISTORY_LIST_SIZE] = {NULL};

    int history_count = 0;

    initGreeting();

    while(1)
    {
        cwd_buffer_coefficient = 1;
        full_cwd_dir = (char*) malloc (
            MAX_BUFFER_CWD * cwd_buffer_coefficient * sizeof(char));

        if (full_cwd_dir == NULL)
        {
            fprintf(stderr, "allocate failed: allocate buffer failed.\n");
            exit(EXIT_FAILURE);
        }

        while (getcwd(full_cwd_dir, MAX_BUFFER_CWD * cwd_buffer_coefficient) == NULL)
        {
            cwd_buffer_coefficient++;
            full_cwd_dir = realloc(
                full_cwd_dir, MAX_BUFFER_CWD * cwd_buffer_coefficient * sizeof(char));
            
            if (full_cwd_dir == NULL)
            {
                fprintf(stderr, "allocate failed: reallocate buffer failed.\n");

                free(full_cwd_dir);

                exit(EXIT_FAILURE);
            }
        }

        cwd_dir = strrchr(full_cwd_dir, '/');

        username = getenv("USER");
        fprintf(stdout, "@%s: ~%s > ", username, cwd_dir);
        fflush(stdout); 
        
        int is_successful = -1;

        input_string = readCmdLine();

        char* temp_cmdline = (char*) malloc (strlen(input_string) + 1);

        if (temp_cmdline == NULL) 
        {
            fprintf(
                stderr, "allocate failed: failed to allocate temp string for %s.\n", 
                input_string);
        }

        strcpy(temp_cmdline, input_string);

        int is_proper_pipe = checkPipeCmd(temp_cmdline);
        // If `is_pipe`:
        // == 0: there is no pipe
        // == 1: there is 1 pipe

        switch (is_proper_pipe)
        {
        case 0: ;    
            char** args_list = parseCmdLine(temp_cmdline, TOKENS_DELIM);

            int type = getCmdType(args_list[0], builtin_cmd_list);
            
            if (type != -1)
                is_successful = executeBuiltinCmdLine(type, args_list, history_list);            
            else
            {
                is_successful = executeBinCmdLine(args_list);
            }

            free(args_list);
            free(temp_cmdline);
            break;
        
        case 1: ;
            char** cmd_list = parseCmdLine(temp_cmdline, TOKEN_PIPE_DELIM);

            char* temp_cmd1 = (char*) malloc (strlen(cmd_list[0]) + 1);

            if (temp_cmd1 == NULL) 
            {
                fprintf(
                    stderr, "allocate failed: failed to allocate temp string for %s.\n", 
                    cmd_list[0]);
            }

            char* temp_cmd2 = (char*) malloc (strlen(cmd_list[1]) + 1);

            if (temp_cmd2 == NULL) 
            {
                fprintf(
                    stderr, "allocate failed: failed to allocate temp string for %s.\n", 
                    cmd_list[1]);
            }

            strcpy(temp_cmd1,cmd_list[0]);
            strcpy(temp_cmd2,cmd_list[1]);

            char** args1_pipe_list = parseCmdLine(temp_cmd1, TOKENS_DELIM);
            char** args2_pipe_list = parseCmdLine(temp_cmd2, TOKENS_DELIM);

            executePipeCmdLine(args1_pipe_list, args2_pipe_list);

            free(cmd_list);
            free(temp_cmdline);

            free(args1_pipe_list);
            free(args2_pipe_list);
            free(temp_cmd1);
            free(temp_cmd2);
            break;

        default:
            break;
        }

        // Add history
        temp_cmdline = (char*) malloc(strlen(input_string) + 1);
                
        if (temp_cmdline == NULL) 
        {
            fprintf(
                stderr, "allocate failed: failed to allocate temp string for %s.\n", 
                input_string);
        }

        strcpy(temp_cmdline, input_string);

        if (is_successful == 0)
            addHistory(temp_cmdline, history_list, &history_count);

        free(input_string);
        free(full_cwd_dir);
    }

    for(int i=0;i<history_count;i++)
    {
        free(history_list[i]);
    }
    
    return EXIT_SUCCESS;
}

int initGreeting(void)
{
    clear(); // ~system("clear");
    printf("******************************************"); 
    printf("\n*THE SIMPLE SHELL PROJECT/OS HCMUS 18CLC3*"); 
    printf("\n******************************************");
    char* username = getenv("USER"); 
    printf("\n\nUSER is: @%s", username);
    printf("\nWelcome!");
    printf("\n"); 

    sleep(1);
    clear(); // ~system("clear");
    return 0;
}

char* readCmdLine(void)
{
    char* line = NULL;
    size_t buffer_size = 0;

    if (getline(&line, &buffer_size, stdin) == -1)
    {
        if (feof(stdin)) 
        {
            fprintf(stdout,"\n");
            exit(EXIT_SUCCESS);
        } 
        else  
        {
            fprintf(stderr, "read failed: readline has failed (EOF).\n"); 
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

int checkPipeCmd(char* line)
{
    char* pipe_checker = NULL;
    
    pipe_checker = strchr(line, '|');

    if(pipe_checker != NULL)
    {
        return 1;
    }
    else return 0;
}

char** parseCmdLine(char* cmdline, char* delims)
{
    int buffer_coefficient = 1;
    int index = 0;

    char** token_list = (char**) malloc (
        TOKENS_BUFFER_SIZE * buffer_coefficient * sizeof(char*));
    
    if (!token_list)
    {
        fprintf(stderr, "allocate failed: failed to allocate dynamic array.\n");
        exit(EXIT_FAILURE);
    }

    char* token = strtok(cmdline, delims);
    while(token != NULL)
    {
        token_list[index] = token;
        index++;

        if (index >= TOKENS_BUFFER_SIZE * buffer_coefficient) 
        {
            buffer_coefficient++;
            token_list = realloc(
                token_list, TOKENS_BUFFER_SIZE * buffer_coefficient * sizeof(char*));
            if (!token_list)
            {
                fprintf(stderr, "allocate failed: failed to reallocate dynamic array.\n");
                exit(EXIT_FAILURE);
            }
        }
    
        token = strtok(NULL, delims);
    } 

    token_list[index] = NULL;

    return token_list;
}

int getCmdType(char* line, char** builtin_list)
{
    // Return 0 : binary, 1 : builtin
    for(int i = 0; i < BUILTIN_LIST_SIZE; i++)
    {
        if(strcmp(builtin_list[i], line) == 0)
            return i;
    }
    return -1;
}

int executeBuiltinCmdLine(int line_index, char** args_list, char** history_list)
{
    switch(line_index)
    {
    case 0: // help
        break;
    case 1: // history
        executeHistoryCmd(history_list);
        break;
    case 2: // cd
        executeChangeDirCmd(args_list);
        break;
    case 3: // exit
        executeExitCmd();
        break;
    default:
        break;
    }
    
    if (line_index == 1)
        return -1;
    else
        return 0;
}

int executeBinCmdLine(char** args_list)
{
    pid_t pid = fork();  
    int child_value = -1;
    
    if (pid < 0) // Forking child process failed
    { 
        fprintf(stderr, "process failed: cannot fork children.\n"); 
    } 
    else 
    {
        if (pid == 0) // Child process
        { 
            if (execvp(args_list[0], args_list) == -1) 
            { 
                fprintf(
                    stderr, "process failed: wrond command or command does not exist.\n");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        } 
        else // Parent process - Waiting for child to terminate 
        { 
            wait(&child_value);  
        }  
    } 

    return child_value / 255;
}

int executePipeCmdLine(char** args1_list, char** args2_list)
{
    int pipe_file_descriptor[2];
    pid_t pid1, pid2;

    if (pipe(pipe_file_descriptor) < 0)
    {
        fprintf(stderr, "process failed: cannot create pipe.\n");
        return 1;
    }

    pid1 = fork();

    if (pid1 < 0) // Failed to fork children
    {
        fprintf(stderr, "process failed: cannot fork children.\n"); 
    }
    else if (pid1 == 0) // Child processing
    {
        close(pipe_file_descriptor[0]);
        dup2(pipe_file_descriptor[1], STDOUT_FILENO);
        close(pipe_file_descriptor[1]);

        if(execvp(args1_list[0], args1_list) == -1) 
        {
            fprintf(stderr, "process failed: wrond command or command does not exist.\n"); 
        }
        exit(EXIT_SUCCESS);
    }
    else // Parent processing
    {
        pid2 = fork();

        if(pid2 < 0) // Failed to fork children
        {
            fprintf(stderr, "process failed: cannot fork children.\n"); 
        }
        else if(pid2 == 0) // Children 2 processing
        {
            close(pipe_file_descriptor[1]);
            dup2(pipe_file_descriptor[0], STDIN_FILENO);
            close(pipe_file_descriptor[0]);
            
            if(execvp(args2_list[0], args2_list) == -1) 
            {
                fprintf(stderr, "process failed: wrond command or command does not exist.\n"); 
            }
            exit(EXIT_SUCCESS);
        }
        else // Parent processing
        {
            close(pipe_file_descriptor[1]);
            wait(NULL);
        }
        close(pipe_file_descriptor[1]);
        wait(NULL);
    }

    return 0;
}

// ==========================================================
// Built-in features
int executeHelpCmd()
{
    
}

int executeHistoryCmd(char** history_list)
{
    for(int i = 0; i < HISTORY_LIST_SIZE; i++)
    {
        if (history_list[i] == NULL)
            break;
        else
            fprintf(stdout, "%d   %s", i + 1, history_list[i]);
    }
}

int executeChangeDirCmd(char** args_list)
{
    if (args_list[1] == NULL) 
    {
        fprintf(stderr, "cd: expected argument to \"cd\".\n");
    } 
    else 
    {
        if (chdir(args_list[1]) == -1) 
        {
            fprintf(stderr, "cd: no such file or directory.\n");
        }
    }
    return 0;
}

int executeExitCmd()
{
    exit(EXIT_SUCCESS);
    return 0;
}

// ==========================================================
// Built-in features

int addHistory(char* input_string, char** history_list, int* count)
{
    if((*count) < 10)
    {
        for(int i = (*count); i >= 0; i--)
        {
            history_list[i] = history_list[i - 1];
        }
        history_list[0] = input_string;
        (*count)++;
    }
    else
    {
        free(history_list[9]);
        for(int i = 9; i >= 0; i--)
        {
            history_list[i] = history_list[i - 1];
        }
        history_list[0] = input_string;
    }
    return 0;
}