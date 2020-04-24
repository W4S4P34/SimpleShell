#include "ShellCore.h"

int main(void)
{
    char* input_string;

    int is_pipe = 0;
    // If `is_pipe`:
    // == 0: there is no pipe
    // == 1: there is 1 pipe    

    init_greeting();

    while(1)
    {
        printf("Trungrancanmo> ");
        fflush(stdout);

        input_string = read_cmdline();

        printf("%s",input_string);

        // is_pipe = check_pipe_cmd(input_string);

        // if(is_pipe)
        // {
        //     char** args_list;
        //     args_list = parse_cmdline(input_string);

        //     free(args_list);
        // }
        // else
        // {
        //     char** args_pipe_list;
        //     args_pipe_list = parse_pipe_cmdline(input_string);

        //     free(args_pipe_list);
        // }

        free(input_string);
    }
    
    return EXIT_SUCCESS;
}

int init_greeting(void)
{
    clear(); // ~system("clear");
    printf("\n******************************************"); 
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

char* read_cmdline(void)
{
    char* line = NULL;
    size_t buffer_size = 0;

    if (getline(&line, &buffer_size, stdin) == -1)
    {
        if (feof(stdin)) 
        {
            exit(EXIT_SUCCESS);
        } 
        else  
        {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

int check_pipe_cmd(char* line)
{

}

char** parse_cmdline(char* line)
{

}

char** parse_pipe_cmdline(char* line)
{

}