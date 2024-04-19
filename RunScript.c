#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int executeScript(const char *scriptName, char **buffer, size_t bufferSize) 
{
    FILE *fp;
    char line[bufferSize];

    fp = popen(scriptName, "r");
    if (fp == NULL) {
        printf("Can't open script\n");
        return 1;
    }

    *buffer = (char *)malloc(bufferSize * sizeof(char));
    if (*buffer == NULL) {
        printf("malloc fail\n");
        return 1;
    }
    (*buffer)[0] = '\0';

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strlen(*buffer) + strlen(line) >= bufferSize) {
            printf("buffer overflow\n");
            break;
        }
        strcat(*buffer, line);
    }

    if (pclose(fp) == -1) {
        printf("Can't close script\n");
        return 1;
    }

    return 0;
}