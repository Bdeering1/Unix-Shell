#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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


/* Adding Background Process Node */
Proc_Node *add_node(char **argv, Proc_Node *head) {
    Proc_Node *node = list_end(head);
    node->next = malloc(sizeof(Proc_Node));
    node->next->argv = argv;
    node->next->bg = true;
    node->next->next = NULL;
    update_node_nums(head);

    return node->next;
}

Proc_Node *list_end(Proc_Node *head) {
    Proc_Node *node = head;
    while (node->next != NULL) {
        node = node->next;
    }
    return node;
}

/* Checking background processes to see if they have finished */
void check_procs(Proc_Node *head) {
    int status;
    int f_num = 0;
    Proc_Node *node = head;
    Proc_Node *prev;
    while (node->next != NULL) {
        prev = node;
        node = node->next;
        if (node->pid == waitpid(node->pid, &status, WNOHANG)) { // process finshed
            printf("[%d]+ Done                    ", node->id_num);
            for (int i = 0; i < MAX_ARGS; i++) {
                if (node->argv[i] == NULL) break;
                printf("%s ", node->argv[i]);
            }
            printf("\n");
            prev->next = node->next;
            node->next = NULL;
        }
    }
}

/* Freeing linked list associated witgh background processes */
void free_proc_list(Proc_Node *head) {
    Proc_Node *node = list_end(head);
    while (node->next != NULL) {
        free(node);
        node = list_end(head);
    }
    free(head);
}

/* Assigns a number to each node based on their order in the list */
void update_node_nums(Proc_Node *head) {
    int id_num = 1;
    Proc_Node *node = head;
    while (node->next != NULL) {
        node = node->next;
        if (node->bg) node->id_num = id_num++;
    }
}


/* Creating args from input */
int create_tokens(char *input, char **arr, int max_size) {
    char *new_str = trim_whitespace(input);

    int num_args = 0;
    char *token = strtok(new_str, " ");
    if (token != NULL)  num_args++;
    int i;
    for(i = 0; i < max_size; i++) {
        arr[i] = token;
        token = strtok(NULL, " ");
        if (token != NULL) {
            num_args++;
        } else {
            break;
        }
    }
    if (token != NULL) {
        printf("-myShell: Too many arguments\n");
        return 0;
    }
    arr[num_args] = NULL;

    return num_args;
}

char *trim_whitespace(char *str) {
    char *end;

    while(isspace((unsigned char)*str))
        str++;

    if(*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

    return str;
}

/* Printing erros (called from functions in execution.c) */
void print_error(int err) {
    if (err == 2) {
        printf("-myShell: No such file or directory\n");
    } else {
        printf("-myShell: Command not found\n");
    }
}