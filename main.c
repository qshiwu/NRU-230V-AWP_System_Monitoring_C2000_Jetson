#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UART.h"
#include "ReadFile.h"

void msleep(int ms)
{
     usleep(1000 * ms);
}

void splitString(const char *input, char delimiter, char **result, int *resultSize) 
{
    char *inputCopy = strdup(input);
    char *token = strtok(inputCopy, &delimiter);
    *resultSize = 0;

    while (token != NULL) {
        result[*resultSize] = strdup(token);
        (*resultSize)++;

        token = strtok(NULL, &delimiter);
    }

    free(inputCopy);
}

int main(int argc, char **argv) 
{
    int fd;
    char buf[256];
    char message[256];
    int CharCnt = 0;
    int GOODCharCnt = 0;
    char *GOOD = "GOOD";
    const char *filename = "UARTCommand.txt";
    char *content = NULL;
    char delimiter = '\n';
    int arrsize = 0;
    char *strarr[2];
    int timecnt = 0;

    if (uart_start(&fd) != 0) {
        return 1;
    }

    strcpy(message, GOOD);
    GOODCharCnt = strlen(message) + 2;
    message[GOODCharCnt - 1] = '\n';
    message[GOODCharCnt] = '\0';

    while(1)
    {
        if((timecnt % 100) == 0)
        {
            if(file_exists(filename))
            {
                if (read_file(filename, &content) == 0) 
                {
                    printf("%s\n", content);
                    splitString(content, delimiter, strarr, &arrsize);
                    free(content);
                }

                for (int i = 0; i < arrsize; i++)
                {
                    strcpy(buf, strarr[i]);
                    CharCnt = strlen(buf) + 2;
                    buf[CharCnt - 1] = '\n';
                    buf[CharCnt] = '\0';
                    write(fd, buf, CharCnt);
                    msleep(1);
                }
                delete_file(filename);
            }
            timecnt = 0;
        }
        timecnt++;
        write(fd, message, GOODCharCnt);
        msleep(9);
    }

    uart_stop(fd);
    
    return 0;
}
