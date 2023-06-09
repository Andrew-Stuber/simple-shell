/* ICCS227: Project 1: icsh
 * Name: Andrew Stuber
 * StudentID: 6281425
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_CMD_BUFFER 255

char buffer[MAX_CMD_BUFFER];
char fg_command[MAX_CMD_BUFFER];
char prev_input[MAX_CMD_BUFFER];
int gac; //global argc to check the input
int fg_pid;
int status; //gets the status of the child
int file;
int out = 1; //stdout
int in = 0;  //stdin

struct job {
    char command[255];
    int is_fg;  // check if it's in foreground
    int is_sus; // check if it's suspended (not amongus)
    int pid;
};

struct job jobs[100];

char echo(char *input[], int pos) {
    for (int i = 0; input[i]; i++) {
        printf("%s%s", input[i], (i == pos - 1) ? "" : " ");
    }
    printf("\n");
    return 0;
}

//write output to file
void write_file(char *f) {
    file = open(f, O_CREAT | O_WRONLY | O_TRUNC);
    if (file < 0) {
        fprintf(stderr, "Couldn't open a file\n");
        start();
    }

    out = dup(1); // stdout -> 4
    file = dup2(file, 1); // file -> 1
}

//read input from file
void read_file(char *f) {
    file = open(f, O_RDONLY);
    if (file < 0) {
        fprintf(stderr, "Couldn't open a file\n");
        start();
    }

    in = dup(0); // stdin -> 4
    file = dup2(file, 0); // file -> 0
}

void background(char input[], char **b) {
    for (int i = 0; i < 100; i++) {
        if (jobs[i].pid == 0) {
            if ((jobs[i].pid = fork()) == 0) {
                setpgid(0, getpid());
                execvp(b[0], b);
            }
           // printf("%d\n", i);
            strcpy(jobs[i].command, input);
            printf("[%d] %s\n", i + 1, jobs[i].command);
            break;
        }
    }
    start();
}

void fg(int id) {
    if (jobs[id - 1].pid == 0) {
        printf("No such job\n");
        return;
    }

    strcpy(fg_command, jobs[id - 1].command);
    int len = strlen(jobs[id - 1].command);
    char command[len];
    strcpy(command, jobs[id - 1].command);
    if (command[len - 1] == '&') {
        command[len - 1] = '\0';
    }

    printf("%s\n", command);
    tcsetpgrp(0, jobs[id - 1].pid);
    kill(jobs[id - 1].pid, SIGCONT);
    jobs[id - 1].is_fg = 1;
    fg_pid = jobs[id - 1].pid;
    jobs[id - 1].pid = 0;
    waitpid(fg_pid, 0, WUNTRACED);
}

void bg(int id) {
    if (jobs[id - 1].pid == 0) {
        printf("No such job\n");
        return;
    }
    jobs[id - 1].is_sus = 0;
    kill(jobs[id - 1].pid, SIGCONT);
}

int command(char input[]) {
    char input_copy[255];
    strcpy(input_copy, input);
    strcpy(fg_command, input);
    if (input[0] == NULL) return 0;

    // get the last word in the string
    char *last_word;
    last_word = strrchr(input, ' ') + 1;

    if (strcmp(input, "echo $?") == 0) {
        //printf("%d\n", status);
        status = 0;
        return 0;
    }

    // keep the previous input if not a !!
    if (strncmp(input, "!!", 2) != 0) {
        strcpy(prev_input, input);
    }
    // string split of 2 sections, the command words and the input string.
    char *a = strtok(input, " ");
    size_t len = strlen(a);
    char command_word[len];
    strcpy(command_word, a);

    char **b = malloc(sizeof(char *) * 255);
    int pos = 0;
    b[pos++] = command_word;
    while (a != NULL) {
        a = strtok(NULL, " ");
        // check if input contains < or >
        if (a && strcmp(a, "<") == 0) {
            read_file(last_word);
            break;
        } else if (a && strcmp(a, ">") == 0) {
            write_file(last_word);
            break;
        }

        if (a && strcmp(a, "&") == 0) {
            b[pos] = NULL;
            background(input_copy, b);
        }

        b[pos++] = a;
    }

    b[pos] = NULL;
    int number = 0;

    // compare the command words to see which one is given.
    if (strcmp(command_word, "echo") == 0) {
        echo(b + 1, pos - 1);
    } else if (strcmp(command_word, "jobs") == 0) {
        // lists the jobs and the status
        for (int i = 0; i < 100; i++) {
            if (jobs[i].pid != 0) {
                printf("[%d]+ %s                  %s\n", i + 1, jobs[i].is_sus ? "Suspended" : "Running", jobs[i].command);
            }
        }
    } else if (strncmp(command_word, "!!", 2) == 0) {
        // if there's no previous input give back prompt
        if (prev_input[0] == '\0') {
            return 0;
        }
        if (gac != 2) printf("%s\n", prev_input);
        command(prev_input);
    } else if (strcmp(command_word, "exit") == 0) {
        if (b[1] == NULL) {
            number = 0;
        } else {
            // convert string to integer and keep number between 0-255
            number = atoi(b[1]) % 256;
        }

        // keep number positive
        if (number < 0) {
            number = number * -1;
        }
        if (gac != 2) printf("bye\n");

        exit(number);
    } else if (strcmp(command_word, "fg") == 0) {
        //get the job id
        int n = atoi(last_word + 1);
        fg(n);
    } else if (strcmp(command_word, "bg") == 0) {
        int n = atoi(last_word + 1);
        bg(n);
    } else {
        if ((fg_pid = fork()) == 0) {
            if (execvp(b[0], b) < 0) {
                printf("%s: command not found\n", b[0]);
            }
        } else {
            // blocks until child is stopped or terminated
            waitpid(fg_pid, 0, WUNTRACED);
            fg_pid = 0;
        }
    }
    if (file) {
        close(file);
        file = 0;
    }
    out = dup2(out, 1); // stdout -> 1
    in = dup2(in, 0); // stdin -> 0
    free(b);
    return 0;
}

void start() {
    while (1) {
        printf("icsh $");
        if (!fgets(buffer, 255, stdin)) break;
        // get rid of new line
        size_t ln = strlen(buffer) - 1;
        if (*buffer && buffer[ln] == '\n')
            buffer[ln] = '\0';

        command(buffer);
        buffer[0] = '\0';
    }
}

// get the signal of the terminated and suspended background job and print it asynchronously
void sig_handler2(int sig, siginfo_t *info, void *trash) {
    tcsetpgrp(0, getpid());
    for (int i = 0; i < 100; i++) {
        if (jobs[i].pid != 0 && info->si_pid == jobs[i].pid) {
            jobs[i].pid = 0;
            jobs[i].is_fg = 0;
            waitpid(jobs[i].pid, 0, WNOHANG);
            printf("\n[%d]+ Done                  %s", i + 1, jobs[i].command);
        }
    }
    printf("\n");

}

void sig_handler(int sig, siginfo_t *info, void *trash) {
    if (fg_pid != 0) {
        if (sig == SIGINT) {
            kill(fg_pid, SIGINT);
        } else if (sig == SIGTSTP) {
            kill(fg_pid, SIGSTOP);
            for (int i = 0; i < 100; i++) {
                if (jobs[i].pid == 0) {
                    strcpy(jobs[i].command, fg_command);
                    jobs[i].pid = fg_pid;
                    jobs[i].is_sus = 1;
                    break;
                }
            }
        }
        fg_pid = 0;
    }
}

// ac checks the len of the input, 1 when there's only ./icsh, 2 when there's a file ie ./icsh test.sh.
// av is the array of character.
int main(int ac, char *av[]) {
    struct sigaction sa;
    struct sigaction sa2;

    sa.sa_sigaction = sig_handler;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigfillset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);

    sa2.sa_sigaction = sig_handler2;
    sa2.sa_flags = SA_RESTART | SA_SIGINFO | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa2, NULL);
    sigaction(SIGTTIN, &sa2, NULL);
    sigaction(SIGTTOU, &sa2, NULL);
    setpgid(0, getpid());
    tcsetpgrp(0, getpid());

    if (ac == 1) {
        gac = 1;
        start();
    } else if (ac == 2) {
        gac = 2;
        FILE *ptr;
        char *line = NULL;
        size_t len = 0;

        ptr = fopen(av[1], "r");
        if (ptr == NULL) {
            exit(EXIT_FAILURE);
        }

        while (getline(&line, &len, ptr) != -1) {
            // get rid of new line
            size_t ln = strlen(line) - 1;
            if (*line && line[ln] == '\n') {
                line[ln] = '\0';
            }
            command(line);
        }

        fclose(ptr);
        if (line) {
            free(line);
        }
        exit(EXIT_SUCCESS);
    }

}
