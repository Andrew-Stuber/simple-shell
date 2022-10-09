/* ICCS227: Project 1: icsh
 * Name: Andrew Stuber
 * StudentID: 6281425
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CMD_BUFFER 255

char prev_input[MAX_CMD_BUFFER];
int gac; //global ac to check the input

char echo(char* input[], int pos){
    for (int i = 0; i < pos - 1; i++) {
        printf("%s%s", *(input + i), (i == pos - 2) ? "" : " ");
    }
    printf("\n");
    return 0;
}

int command(char input[]){
    // keep the previous input if not a !!
    if (strncmp(input, "!!", 2) != 0) {
        strcpy(prev_input, input);
    }
    // string split of 2 sections, the command words and the input string.
    char* a = strtok(input, " ");
    size_t len = strlen(a);
    char command_word[len];
    strcpy(command_word, a);

    if (a[len - 1] == '\n') {
        command_word[len - 1] = 0;
    }

    char** b = malloc(sizeof(char*)*255);
    int pos = 0;
    b[pos++] = command_word;
    while (a != NULL){
        a = strtok(NULL, " ");
        b[pos++] = a;
    }

    b[pos] = NULL;
    int number = 0;

    // compare the command words to see which one is given.
    if(strcmp(command_word, "echo") == 0){
        echo(b + 1, pos - 1);
    } else if(strncmp(command_word, "!!", 2) == 0) {
        // if there's no previous input give back prompt
        if (prev_input[0] == '\0') {
            return 0;
        }
        if (gac != 2) printf("%s\n", prev_input);
        command(prev_input);
    } else if(strcmp(command_word, "exit") == 0){
        if (b[1] == NULL) {
            number = 0;
        }
        else {
            // convert string to integer and keep number between 0-255
            number = atoi(b[1]) % 256;
        }

        // keep number positive
        if(number < 0){
            number = number * -1;
        }
        if (gac != 2) printf("bye\n");

        exit(number);
    } else {
        int status;

        if(fork() == 0){
            if (execvp(b[0], b) < 0){
                printf("%s: command not found\n", b[0]);
            }
        } else {
            wait(&status);
        }
    }

    free(b);
    return 0;
}

// ac checks the len of the input, 1 when there's only ./icsh, 2 when there's a file ie ./icsh test.sh.
// av is the array of character.
int main(int ac, char *av[]) {
    char buffer[MAX_CMD_BUFFER];

    if(ac == 1) {
        gac = 1;
        while (1) {
            printf("icsh $");
            fgets(buffer, 255, stdin);

            // get rid of new line
            size_t ln = strlen(buffer) - 1;
            if (*buffer && buffer[ln] == '\n')
                buffer[ln] = '\0';

            //printf("%s\n", buffer);

            command(buffer);
        }
    } else if(ac == 2){
        gac = 2;
        FILE *ptr;
        char *line = NULL;
        size_t len = 0;

        ptr = fopen(av[1], "r");
        if(ptr == NULL){
            exit(EXIT_FAILURE);
        }

        while(getline(&line, &len, ptr) != -1) {
            // get rid of new line
            size_t ln = strlen(line) - 1;
            if (*line && line[ln] == '\n')
                line[ln] = '\0';

            command(line);
        }

        fclose(ptr);
        if(line){
            free(line);
        }
        exit(EXIT_SUCCESS);
    }

}
