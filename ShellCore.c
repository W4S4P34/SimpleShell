#include "ShellCore.h"

int main(void)
{
    char* input_string;  

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
            char** args_list = parseCmdLine(input_string);
            executeCmdLine(args_list);

            // printf("0\n");

            free(args_list);
            break;
        
        case 1: ;
            // char** args_pipe_list;
            // args_pipe_list = parsePipeCmdLine(input_string);

            printf("1\n");

            // free(args_pipe_list);
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

char** parseCmdLine(char* cmdline)
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

    char* token = strtok(cmdline, TOKENS_DELIM);
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
    
        token = strtok(NULL, TOKENS_DELIM);
    } 

    return token_list;
}

char** parsePipeCmdLine(char* line)
{

}

int executeCmdLine(char** args_list)
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