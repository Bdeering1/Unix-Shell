#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <signal.h>
#include <stdbool.h>
#include "defs.h"

/*
 *      CIS*3110 Assignment #1
 *      Unix Shell
 *
 *      Author: Bryn Deering
 *      Date of Last Revision: February 12, 2020
 * 
 */

/* Regular or Background Execution */
int execute(Proc_Node *proc) {
    int status;
    pid_t child_pid;

    if ((child_pid = fork()) == 0) { // child process
        status = execvp(*proc->argv, proc->argv);
        print_error(errno);
        exit(0);
    } else if (child_pid > 0) { // parent process
        proc->pid = child_pid;
        if (proc->bg) {
            printf("[%d] %d\n", proc->id_num, child_pid);
        } else {
            waitpid(child_pid , &status, 0);
        }
    } else {
        perror("fork");
        exit(-1);
    }
    return WEXITSTATUS(status);
}

/* Execution with Redirected Input or Output */
int file_exec(Proc_Node *proc, char *filename, int mode) {
    int status;
    pid_t child_pid;

    if ((child_pid = fork()) == 0) { // child process

        /* changing input */
        if (mode == READ_MODE && freopen(filename, "r", stdin) == NULL) {
            printf("-myShell: File not found\n");
            exit(0);
        }
        /* changing output */
        if (mode == WRITE_MODE && freopen(filename, "w", stdout) == NULL) {
            printf("-myShell: File not found\n");
            exit(0);
        }
        
        status = execvp(*proc->argv, proc->argv);
        print_error(errno);
        exit(0);
    } else if (child_pid > 0) { //parent process

        waitpid(child_pid , &status, 0);

    } else {
        perror("fork");
        exit(-1);
    }
    return WEXITSTATUS(status);
}

int pipe_exec(Proc_Node *proc_1, Proc_Node *proc_2) {
    pid_t child_pid_1, child_pid_2;

    /* creating pipe */
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(-1);
    }

    if ((child_pid_1 = fork()) < 0) {
        perror("fork");
        exit(-1);
    }
    if (child_pid_1 == 0) { // first child process
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(*proc_1->argv, proc_1->argv);
        print_error(errno);
        exit(0);
    }
    /* parent process (before creating second child) */
    if ((child_pid_2 = fork()) < 0) {
        perror("fork");
        exit(-1);
    }
    if (child_pid_2 == 0) { // second child process
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(*proc_2->argv, proc_2->argv);
        print_error(errno);
        exit(0);
    }
    /* parent process (after creating second child) */
    close(fd[0]);
    close(fd[1]);
    waitpid(child_pid_1, NULL, 0);
    waitpid(child_pid_2, NULL, 0);

    return 0;
}