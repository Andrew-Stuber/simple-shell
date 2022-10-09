/* ICCS227: Project 1: icsh
 * Name: Andrew Stuber
 * StudentID: 6281425
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CMD_BUFFER 255

char prev_input[MAX_CMD_BUFFER];
int gac; //global ac to check the input

char echo(char* input[], int pos){
    for (int i = 0; i < pos - 1; i++) {
        printf("%s%s", *(input + i), (i == pos - 2) ? "" : " ");
    }
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

    char** b = (char**)malloc(sizeof(char*)*255);
    int pos = 0;
    while (a != NULL){
        a = strtok(NULL, " ");
        b[pos++] = a;
    }

    int number = 0;

    // compare the command words to see which one is given.
    if(strcmp(command_word, "echo") == 0){
        echo(b, pos);
    } else if(strncmp(command_word, "!!", 2) == 0) {
        // if there's no previous input give back prompt
        if (prev_input[0] == '\0') {
            return 0;
        }
        if (gac != 2) printf("%s", prev_input);
        command(prev_input);
    } else if(strcmp(command_word, "exit") == 0){
        // convert string to integer and keep number between 0-255
        number = atoi(b[0])%256;

        // keep number positive
        if(number < 0){
            number = number * -1;
        }
        if (gac != 2) printf("bye\n");

        exit(number);
    } else {
        printf("Command not found\n");
    }

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
            command(line);
        }

        fclose(ptr);
        if(line){
            free(line);
        }
        exit(EXIT_SUCCESS);
    }
}
