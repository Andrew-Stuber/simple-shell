/* ICCS227: Project 1: icsh
 * Name: Andrew Stuber
 * StudentID: 6281425
 */

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define MAX_CMD_BUFFER 255

int command(char input[]){
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
        for (int i = 0; i < pos - 1; i++) {
            printf("%s%s", *(b + i), (i == pos - 2) ? "" : " ");
        }
    } else if(strncmp(command_word, "!!", 2) == 0) {
        bash(last_command);
    } else if(strcmp(command_word, "exit") == 0){
        // convert string to integer and keep number between 0-255
        number = atoi(b)%256;
        // keep number positive
        if(number < 0){
            number = number * -1;
        }
        printf("bye");
        exit(number);
    } else {
        printf("Command not found\n");
    }

    return 0;
}

int main() {
    char buffer[MAX_CMD_BUFFER];
    while (1) {
        printf("icsh $");
        fgets(buffer, 255, stdin);
        //printf("%s\n", buffer);
        command(buffer);
    }
}
