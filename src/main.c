#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "defs.h"

/*
 *      CIS*3110 Assignment #1
 *      Unix Shell
 *
 *      Author: Bryn Deering
 *      Date of Last Revision: February 12, 2020
 *               
 */



int main()
{
    bool run_shell = true;

    size_t length = 1000;
    char *input = (char *)malloc(sizeof(char) *length);
    int num_args;
    char *argv[MAX_ARGS];

    Proc_Node *head = (Proc_Node *)malloc(sizeof(Proc_Node)); //process information is stored in structs (allows for linked list of background processes)
    Proc_Node proc_1, proc_2;

    printf("\n\nWelcome to Bryn's shell.\n");

    while (run_shell) {

        /* Checking for completion of background processes */
        check_procs(head);

        /* Reading input */
        printf("> ");
        fgets(input, length, stdin);

        /* Dividing user input by spaces */
        num_args = create_tokens(input, argv, MAX_ARGS);

        if (num_args == 0) { //blank input or too many arguments
            continue;
        } else if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "quit") == 0) //exit command
        {
            if (argv[1] == NULL) run_shell = false;
            signal(SIGCHLD, SIG_IGN); //make kernel collect any running processes or zombies
        }
        else if (strcmp(argv[num_args - 1], "&") == 0) //execute in the background
        {
            argv[num_args - 1] = NULL;
            execute(add_node(argv, head));
        }
        else
        {
            int arg = 0;
            for (arg = 0; arg < num_args; arg++) {
                if (strcmp(argv[arg], "<") == 0) { //redirect input
                    if (arg == num_args - 1) {
                        printf("\nInvalid arguments: please enter a file name to direct input to\n");
                        break;
                    }
                    argv[arg] = NULL;
                    proc_1.argv = argv;
                    file_exec(&proc_1, argv[arg + 1], READ_MODE);
                    break;
                } else if (strcmp(argv[arg], ">") == 0) { //redirect output
                    if (arg == num_args - 1) {
                        printf("\nInvalid arguments: please enter a file name to direct output to\n");
                        break;
                    }
                    argv[arg] = NULL;
                    proc_1.argv = argv;
                    file_exec(&proc_1, argv[arg + 1], WRITE_MODE);
                    break;
                } else if (strcmp(argv[arg], "|") == 0) { //pipe output to another process
                    argv[arg] = NULL;
                    proc_1.argv = argv;
                    proc_2.argv = &argv[arg + 1];

                    pipe_exec(&proc_1, &proc_2);
                    break;
                }
            }

            if (arg == num_args) { //execute normally
                proc_1.argv = argv;
                proc_1.bg = false;
                execute(&proc_1);
            }
        }       

        update_node_nums(head); //ensuring background processes have the correct numbers assigned
    }

    free(input);
    free_proc_list(head);
    printf("\n");
    return 0;
}







