#include "ShellCore.h"

int main(void)
{
    char* username = NULL, * full_cwd_dir = NULL, * cwd_dir = NULL;
    char* history_cmdline = NULL;
    int cwd_buffer_coefficient;

    int command_check; // Check whether command is valid or not

    char* input_string = NULL;

    int args_count = 0;  
    int args1_count = 0;  
    int args2_count = 0;  

    
    char* builtin_cmd_list[BUILTIN_LIST_SIZE] = {"help", "history", "cd", "exit"};

    char* history_list[HISTORY_LIST_SIZE] = {NULL};
    int history_count = 0;

    int type; // Type of commands: Built-in or Binary

    initGreeting();

    username = getenv("USER");

    while(1)
    {
        // Get information for initial display
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
            char* new_full_cwd_dir = realloc(
                full_cwd_dir, MAX_BUFFER_CWD * cwd_buffer_coefficient * sizeof(char));
            
            if (new_full_cwd_dir == NULL)
            {
                fprintf(stderr, "allocate failed: reallocate buffer failed.\n");

                free(full_cwd_dir);
                exit(EXIT_FAILURE);
            }
            else
            {
                full_cwd_dir = new_full_cwd_dir;
            }           
        }

        cwd_dir = strrchr(full_cwd_dir, '/');

        fprintf(stdout, "@%s: ~%s > ", username, cwd_dir);
        fflush(stdout); 
        
        // Main section
        command_check = 1; // 1 == error, 0 == successful

        input_string = readCmdLine();

        //// Add history
        history_cmdline = (char*) malloc (strlen(input_string) + 1);
                
        if (history_cmdline == NULL) 
        {
            fprintf(
                stderr, "allocate failed: failed to allocate temp string for %s.\n", 
                input_string);

            free(input_string);
            free(full_cwd_dir);
            break;
        }

        strcpy(history_cmdline, input_string);
    
        addHistory(history_cmdline, history_list, &history_count);

        //// Create temp string to manipulate
        char *temp_cmdline = (char*) malloc (strlen(input_string) + 1);

        if (temp_cmdline == NULL) 
        {
            fprintf(
                stderr, 
                "allocate failed: failed to allocate temp string for %s.\n", 
                input_string);

            free(input_string);
            free(full_cwd_dir);
            break;
        }

        if (input_string[0] == '!')
        {
            int history_no = handleExclamation(input_string, history_count);
            if(history_count != 0)
            {
                if(history_no != -1 && history_list[history_no - 1] != NULL)
                {
                    strcpy(temp_cmdline, history_list[history_no - 1]);
                    fprintf(stdout, "%s", temp_cmdline);
                }
                else
                {
                    fprintf(stderr,"history: access violation!\n");

                    free(input_string);
                    free(full_cwd_dir);
                    free(temp_cmdline);
                    continue;
                }
            }
            else
            {
                fprintf(stderr,"history: history is empty!\n");

                free(input_string);
                free(full_cwd_dir);
                free(temp_cmdline);
                continue;
            }
        }
        else
        {
            strcpy(temp_cmdline, input_string);
        }

        int ampersand_no = checkAmpersandCmd(temp_cmdline);

        if (ampersand_no > 1)
        {
            fprintf(stderr,"ampersand: too many ampersand options!\n");

            free(input_string);
            free(full_cwd_dir);
            free(temp_cmdline);
            continue;
        }
        else if (ampersand_no == -1)
        {
            fprintf(stderr,"ampersand: place ampersand at wrong position!\n");

            free(input_string);
            free(full_cwd_dir);
            free(temp_cmdline);
            continue;
        }

        int greater_ope_no = checkRedGreCmd(temp_cmdline);
        int smaller_ope_no = checkRedSmaCmd(temp_cmdline);

        int is_proper_redirect = 1; // 1 is not proper, 0 is proper, -1 is no redirect

        if (ampersand_no > 0 && greater_ope_no > 0
                || ampersand_no > 0 && smaller_ope_no > 0)
        {
            fprintf(stderr,"ampersand and redirect: redirect not support ampersand!\n");

            free(input_string);
            free(full_cwd_dir);
            free(temp_cmdline);
            continue;
        }
        else if (greater_ope_no > 1 || smaller_ope_no > 1
                    || greater_ope_no == 1 && smaller_ope_no > 0
                    || greater_ope_no > 0 && smaller_ope_no == 1)
        {
            fprintf(stderr,"redirect: too many redirect options!\n");

            free(input_string);
            free(full_cwd_dir);
            free(temp_cmdline);
            continue;
        }
        else if (greater_ope_no == 0 && smaller_ope_no == 0)
        {
            is_proper_redirect = -1;
        }
        else
        {
            is_proper_redirect = 0;
        }
        
        int pipe_no = checkPipeCmd(temp_cmdline);
        // If `is_pipe`:
        // == 0: there is no pipe
        // == 1: there is 1 pipe
        // else: cannot handle

        if (pipe_no > 0 && is_proper_redirect == 0)
        {
            fprintf(stderr,"redirect and pipe: pipe not support redirect!\n");

            free(input_string);
            free(full_cwd_dir);
            free(temp_cmdline);
            continue;
        }
        else if (pipe_no > 0 && ampersand_no > 0)
        {
            fprintf(stderr,"ampersand and pipe: pipe not support ampersand!\n");

            free(input_string);
            free(full_cwd_dir);
            free(temp_cmdline);
            continue;
        }

        switch (pipe_no)
        {
        case 0: ;    
            char** args_list = parseCmdLine(temp_cmdline, TOKENS_DELIM, &args_count);

            type = getCmdType(args_list[0], builtin_cmd_list);

                        
            if (type == -1)
            {
                if (is_proper_redirect == 0)
                {
                    char file[strlen(args_list[args_count - 1]) + 1];

                    strcpy(file,  args_list[args_count - 1]);

                    args_list[args_count - 1] = NULL;

                    command_check = executeBinCmdLine(args_list,
                                        smaller_ope_no, greater_ope_no, 
                                        file,
                                        ampersand_no);
                }
                else if (is_proper_redirect == -1)
                {
                    command_check = executeBinCmdLine(args_list,
                                        smaller_ope_no, greater_ope_no, 
                                        NULL,
                                        ampersand_no);
                }
            }
            else if (type != -1 && ampersand_no == 0 && is_proper_redirect == -1)
            {
                command_check = executeBuiltinCmdLine(type, args_list, history_list);
            }
            else
            {               
                fprintf(stderr, "builtin: builtin not support redirect or ampersand.\n");
            }
            
            free(args_list);
            free(temp_cmdline);
            break;
        
        case 1: ;
            char** cmd_list = parseCmdLine(temp_cmdline, TOKEN_PIPE_DELIM, &args_count);

            char* temp_cmd1 = (char*) malloc (strlen(cmd_list[0]) + 1);

            if (temp_cmd1 == NULL) 
            {
                fprintf(
                    stderr, "allocate failed: failed to allocate temp string for %s.\n", 
                    cmd_list[0]);

                exit(EXIT_FAILURE);
            }

            char* temp_cmd2 = (char*) malloc (strlen(cmd_list[1]) + 1);

            if (temp_cmd2 == NULL) 
            {
                fprintf(
                    stderr, "allocate failed: failed to allocate temp string for %s.\n", 
                    cmd_list[1]);

                exit(EXIT_FAILURE);
            }

            strcpy(temp_cmd1,cmd_list[0]);
            strcpy(temp_cmd2,cmd_list[1]);

            char** args1_pipe_list = parseCmdLine(temp_cmd1, TOKENS_DELIM, &args1_count);
            char** args2_pipe_list = parseCmdLine(temp_cmd2, TOKENS_DELIM, &args2_count);

            type = getCmdType(args1_pipe_list[0], builtin_cmd_list);

            if (type != -1)
                fprintf(stderr, "pipe: built-in cmd does not support pipe.\n");
            else
            {
                command_check = executePipeCmdLine(args1_pipe_list, args2_pipe_list);
            }

            free(cmd_list);
            free(temp_cmdline);

            free(args1_pipe_list);
            free(args2_pipe_list);
            free(temp_cmd1);
            free(temp_cmd2);
            break;

        default:
            fprintf(stderr, "pipe: too many pipes, cannot handle.\n");
            break;
        }

        free(input_string);
        free(full_cwd_dir);

        if (command_check == -1)
            break;
    }

    for(int i = 0; i < history_count; i++)
    {
        free(history_list[i]);
    }

    exit(EXIT_SUCCESS);
}

// ==========================================================

int initGreeting(void)
{
    clear(); // ~system("clear");
    printf("******************************************\n"); 
    printf("*THE SIMPLE SHELL PROJECT/OS HCMUS 18CLC3*\n"); 
    printf("******************************************\n\n");

    char* username = getenv("USER"); 
    printf("USER is: @%s\n\n", username);

    printf("Welcome to simple shell!\n");

    sleep(1);
    clear(); // ~system("clear");
    return 0;
}

// ==========================================================
// Core Shell

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

int checkAmpersandCmd(char* line)
{
    int ampersand_counter = 0;
    
    int i = 0;

    while(line[i] != '\0')
    {
        if (line[i] == '&' && line[i + 1] == '\n')
        {
            ampersand_counter++;
        }
        else if (line[i] == '&' && line[i + 1] != '\n')
        {
            return -1;
        }
        
        i++;
    }

    return ampersand_counter;
}

int checkRedGreCmd(char* line)
{
    int greaterope_counter = 0;
    
    int i = 0;

    while(line[i] != '\0')
    {
        if (line[i] == '>')
        {
            greaterope_counter++;
        }

        i++;
    }

    return greaterope_counter;
}

int checkRedSmaCmd(char* line)
{
    int smallerope_counter = 0;
    
    int i = 0;

    while(line[i] != '\0')
    {
        if (line[i] == '<')
        {
            smallerope_counter++;
        }

        i++;
    }

    return smallerope_counter;
}

int checkPipeCmd(char* line)
{
    int pipe_counter = 0;
    
    int i = 0;

    while(line[i] != '\0')
    {
        if (line[i] == '|')
        {
            pipe_counter++;
        }

        i++;
    }

    return pipe_counter;
}

char** parseCmdLine(char* cmdline, char* delims, int* index)
{
    int buffer_coefficient = 1;
    (*index) = 0;

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
        token_list[(*index)] = token;
        (*index)++;

        if ((*index) >= TOKENS_BUFFER_SIZE * buffer_coefficient) 
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

    token_list[(*index)] = NULL;

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
        executeHelpCmd(args_list);
        break;
    case 1: // history
        executeHistoryCmd(args_list, history_list);
        break;
    case 2: // cd
        executeChangeDirCmd(args_list);
        break;
    case 3: // exit
        executeExitCmd(args_list);
        break;
    default:
        break;
    }
    
    if (line_index == 3)
        return -1;
    else
        return 0;
}

int executeBinCmdLine(char** args_list, 
                        int is_proper_smaller, int is_proper_greater,
                        char* file,
                        int ampersand_count)
{
    pid_t wpid;
    int status;
    
    pid_t pid = fork();

    if (pid < 0) // Forking child process failed
    { 
        fprintf(stderr, "process failed: cannot fork children.\n"); 
        return 1;
    } 
    else 
    {
        if (pid == 0) // Child process
        { 
            if (is_proper_smaller == 1)
            {
                int in_fd = open(file, O_RDONLY);

                if (in_fd == -1)
                {
                    fprintf(stderr, "file: cannot open file.\n");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    dup2(in_fd, STDIN_FILENO);
                    close(in_fd);
                }
            }

            if (is_proper_greater == 1)
            {
                int out_fd = creat(file , 0644);

                if (out_fd == -1)
                {
                    fprintf(stderr, "file: cannot create file.\n");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    dup2(out_fd, STDOUT_FILENO);
                    close(out_fd);
                }
            }

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
            if (ampersand_count == 0)
            {
                // Case normal
                wpid = waitpid(pid, &status, 0); 
            }
            // Case bg
            else
            {
                fprintf(stdout, "[1] %d\n", pid);
                wpid = waitpid(pid, &status, WUNTRACED);
                if(WIFEXITED(status))
                {
                    fprintf(stdout, "[1]+ Done\n");
                }
            }
        }  
    }     

    return 0;
}

int executePipeCmdLine(char** args1_list, char** args2_list)
{
    int pipe_file_descriptor[2];
    pid_t pid1, pid2, wpid;
    int status;

    if (pipe(pipe_file_descriptor) < 0)
    {
        fprintf(stderr, "process failed: cannot create pipe.\n");
        return 1;
    }

    pid1 = fork();

    if (pid1 < 0) // Failed to fork children
    {
        fprintf(stderr, "process failed: cannot fork children.\n"); 
        return 1;

    }
    else if (pid1 == 0) // Child processing
    {
        close(pipe_file_descriptor[0]);
        dup2(pipe_file_descriptor[1], STDOUT_FILENO);
        close(pipe_file_descriptor[1]);

        if(execvp(args1_list[0], args1_list) == -1) 
        {
            fprintf(stderr, "process failed: wrond command or command does not exist.\n"); 
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else // Parent processing
    {
        pid2 = fork();

        if(pid2 < 0) // Failed to fork children
        {
            fprintf(stderr, "process failed: cannot fork children.\n"); 
            return 1;
        }
        else if(pid2 == 0) // Children 2 processing
        {
            close(pipe_file_descriptor[1]);
            dup2(pipe_file_descriptor[0], STDIN_FILENO);
            close(pipe_file_descriptor[0]);
            
            if(execvp(args2_list[0], args2_list) == -1) 
            {
                fprintf(stderr, "process failed: wrond command or command does not exist.\n");
                exit(EXIT_FAILURE); 
            }
            exit(EXIT_SUCCESS);
        }
        else // Parent processing
        {
            close(pipe_file_descriptor[1]);
            wpid = waitpid(pid1, &status, 0);
            wpid = waitpid(pid2, &status, 0);
        }        
    }

    return 0;
}

// ==========================================================
// Built-in features
int executeHelpCmd(char** args_list)
{
    if(args_list[1] == NULL)
    {
        fprintf(stdout, "SHELL HELP\n"
                        "List of commands supported:\n"
                        "> cd\n"
                        "> history (with exclamation operator '!')\n"
                        "> exit\n"
                        "> Redirect with '>' and '<' operators\n"
                        "> General commands in UNIX Shell\n"
                        "> Pipe handling\n"
                                    );
    }
    else
    {
        fprintf(stderr, "help: too many arguments.\n");
    }
    
    return 0;
}

int executeHistoryCmd(char** args_list, char** history_list)
{
    if(args_list[1] == NULL)
        for(int i = 0; i < HISTORY_LIST_SIZE; i++)
        {
            if (history_list[i] == NULL)
                break;
            else
                fprintf(stdout, "%d   %s", i + 1, history_list[i]);
        }
    else
    {
        fprintf(stderr, "history: too many arguments.\n");
    }
    
    return 0;
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

int executeExitCmd(char** args_list)
{
    if(args_list[1] != NULL)
    {
        fprintf(stderr, "exit: too many arguments.\n");
    }
    return 0;
}

// ==========================================================
// Sub features

int addHistory(char* input_string, char** history_list, int* count)
{
    if(input_string[0] == '!')
        return 0;
    else
        if((*count) < 10)
        {
            history_list[(*count)] = input_string;
            (*count)++;
        }
        else
        {
            free(history_list[0]);
            for(int i = 0; i < (*count) - 1; i++)
            {
                history_list[i] = history_list[i + 1];
            }
            history_list[9] = input_string;
        }
    return 0;
}

int handleExclamation(char* input_string, int count)
{
    if (strlen(input_string) > 4)
    {
        fprintf(stderr, "history: too many arguments.\n");
        return -1;
    }
    else if (strlen(input_string) == 2)
    {
        fprintf(stderr, "history: wrong command.\n");
        return -1;
    }
    else
    {
        if(strlen(input_string) == 3 && input_string[1] == '!')
            return count;
        else if(strlen(input_string) == 3 && 
            (int)((input_string[1]) - 48) > 0 && (int)((input_string[1]) - 48) <= 9)
        {
            return (int)((input_string[1]) - 48);
        }
        else if(strlen(input_string) == 4 && 
            (int)((input_string[1]) - 48) == 1 && (int)((input_string[2]) - 48) == 0)
        {
            return 10; 
        }
        else
        {
            return -1;
        }
    }
}