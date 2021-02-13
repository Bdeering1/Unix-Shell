#ifndef __DEFSH__
#define __DEFSH__
#endif

#include <sys/types.h>
#include <stdbool.h>

/*
 *      CIS*3110 Assignment #1
 *      Unix Shell
 *
 *      Author: Bryn Deering
 *      Date of Last Revision: February 12, 2020
 * 
 */


/* Maximum arguments allowed in argv */
#define MAX_ARGS 32
#define MAX_PROCS 20
#define READ_MODE 1
#define WRITE_MODE 2

/* Process Struct */
typedef struct Process_Node {
    pid_t pid;
    char **argv;
    bool bg;
    int id_num;
    struct Process_Node *next;
} Proc_Node;

/* Utlity Functions */
Proc_Node *add_node(char **argv, Proc_Node *head);
Proc_Node *list_end(Proc_Node *head);
void check_procs(Proc_Node *head);
void free_proc_list(Proc_Node *head);
void update_node_nums(Proc_Node *head);

int create_tokens(char *input, char **arr, int size);
char *trim_whitespace();
void print_error(int err);


/* Commands Execution */
int execute(Proc_Node *proc);
int file_exec(Proc_Node *proc, char *filename, int mode);
int pipe_exec(Proc_Node *proc_1, Proc_Node *proc_2);