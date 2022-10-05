/* ICCS227: Project 1: icsh
 * Name: Andrew Stuber
 * StudentID: 6281425
 */

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define MAX_CMD_BUFFER 255

char *last_command;

int bash(char input[], char* output[]){
    printf("%s\n", input);
    printf("%s\n", output[0]);
    return 0;
}

int command(char input[]){
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

    if(strcmp(command_word, "echo") == 0){
        for (int i = 0; i < pos - 1; i++) {
            printf("%s%s", *(b + i), (i == pos - 2) ? "" : " ");
        }
    } else if(strncmp(command_word, "!!", 2) == 0) {
        bash(input ,b);
    } else if(strcmp(command_word, "exit") == 0){

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
