/* ICCS227: Project 1: icsh
 * Name: Andrew Stuber
 * StudentID: 6281425
 */

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define MAX_CMD_BUFFER 255

int bash(char input[], char* output[]){
    printf("%s\n", input);
    printf("%s\n", output[0]);
    return 0;
}

int command(char input[]){
    //char a[] - immutable => a[0] ='h' NOT OK
    //char* a - mutable => a[0] = 'h' OK
    char* a = strtok(input, " ");
    char command_word[strlen(a)];
    strcpy(command_word, a);
    char** b = (char**)malloc(sizeof(char*)*255);
    int pos = 0;
    while (a != NULL){
        a = strtok(NULL, " ");
        b[pos] = a;
        pos++;
    }

    if(strcmp(command_word, "echo") == 0){
        int new_pos = pos - 2;
        int last_str_len = strlen(b[new_pos]);
        char last_str[last_str_len];
        strncpy(last_str, b[new_pos], last_str_len);
        b[new_pos] = last_str;
        b[new_pos][last_str_len] = '\0';

        for(int i = 0; i < pos-1; i++){
            printf("%s ", b[i]);
        }
        printf("\n");
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
