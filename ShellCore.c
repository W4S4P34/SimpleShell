#include "ShellCore.h"

int main(void)
{
    char* input_string;  
    
    char* builtin_cmd_list[BUILTIN_LIST_SIZE] = {"help", "history", "cd", "exit"};

    char* history_list[HISTORY_LIST_SIZE];

    initGreeting();

    while(1)
    {
        printf("Trungrancanmo> ");
        fflush(stdout);

        input_string = readCmdLine();

        int is_proper_pipe = checkPipeCmd(input_string);
        // If `is_pipe`:
        // == 0: there is no pipe
        // == 1: there is 1 pipe

        switch (is_proper_pipe)
        {
        case 0: ;
            char** args_list = parseCmdLine(input_string, TOKENS_DELIM);

            int type = getCmdType(args_list[0], builtin_cmd_list);
            
            if (type != -1)
                executeBuiltinCmdLine(type, args_list);            
            else
            {
                executeBinCmdLine(args_list);
            }

            free(args_list);
            break;
        
        case 1: ;
            char** cmd_list = parseCmdLine(input_string, TOKEN_PIPE_DELIM);
            char** args1_pipe_list = parseCmdLine(cmd_list[0], TOKENS_DELIM);
            char** args2_pipe_list = parseCmdLine(cmd_list[1], TOKENS_DELIM);

            for (int i = 0; i < 2 ; i++)
            {
                printf("%s ", args1_pipe_list[i]);
            }

            printf("\n");

            for (int i = 0; i < 2 ; i++)
            {
                printf("%s ", args2_pipe_list[i]);
            }

            printf("\n");

            free(cmd_list);
            free(args1_pipe_list);
            free(args2_pipe_list);
            break;

        default:
            break;
        }

        free(input_string);
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

    sleep(2);
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
            printf("\n");
            exit(EXIT_SUCCESS);
        } 
        else  
        {
            perror("readline");
            printf("\n");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

int checkPipeCmd(char* line)
{
    char* pipe_checker = NULL;
    int pipe_counter = 0;

    
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
    int token_count = 0;

    char** token_list = (char**) malloc (
        TOKENS_BUFFER_SIZE * buffer_coefficient * sizeof(char*));
    
    if (!token_list)
    {
        printf("Failed to allocate dynamic array.\n");
        exit(EXIT_FAILURE);
    }

    char *temp_cmdline = (char*) malloc (strlen(cmdline) + 1);

    if (temp_cmdline == NULL) 
    {
        fprintf(stderr, "parse: failed to allocate temp string for %s\n", cmdline);
    }

    strcpy(temp_cmdline, cmdline);

    char* token = strtok(temp_cmdline, delims);
    while(token != NULL)
    {
        token_count++;
        token_list[token_count - 1] = token;

        if (token_count - 1 >= TOKENS_BUFFER_SIZE * buffer_coefficient) 
        {
            buffer_coefficient++;
            token_list = realloc(
                token_list, TOKENS_BUFFER_SIZE * buffer_coefficient * sizeof(char*));
            if (!token_list)
            {
                printf("Failed to reallocate dynamic array.\n");
                exit(EXIT_FAILURE);
            }
        }
    
        token = strtok(NULL, delims);
    } 

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

int executeBuiltinCmdLine(int line_index, char** args_list)
{
    switch(line_index)
    {
    case 0: // help
        break;
    case 1: // history
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

    return 0;
}

int executeBinCmdLine(char** args_list)
{
    pid_t pid = fork();  
  
    if (pid == -1) // Forking child process failed
    { 
        printf("Forking child process failed.\n"); 
        return 0; 
    } 
    else 
    {
        if (pid == 0) 
        { 
            if (execvp(args_list[0], args_list) == -1) 
            { 
                printf("Wrong command. Please check again.\n"); 
            }
            exit(0);
        } 
        else // Waiting for child to terminate 
        { 
            wait(NULL);  
            return 1; 
        }  
    } 

    return 1;
}

int executePipeCmdLine(char** args_list)
{
    
    return 0;
}

// ==========================================================
// Built-in features
int executeHelpCmd()
{
    printf("I am Iron Man\n");
}

int executeHistoryCmd()
{

}

int executeChangeDirCmd(char** args_list)
{
    if (args_list[1] == NULL) 
    {
        fprintf(stderr, "cd: expected argument to \"cd\"\n");
    } 
    else 
    {
        if (chdir(args_list[1]) == -1) 
        {
            perror("cd");
        }
    }
    return 1;
}

int executeExitCmd()
{
    exit(EXIT_SUCCESS);
    return 1;
}
