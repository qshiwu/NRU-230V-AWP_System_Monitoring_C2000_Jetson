#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UART.h"
#include "RunScript.h"
#include <pthread.h>

#define MAX_BUFFER_SIZE 1024

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

void *thread_function1(void *arg) {
    int CharCnt = 0;
    char buf[256];
    char *content = NULL;
    char delimiter = '\n';
    int arrsize = 0;
    char *strarr[2];
    int fd = *((int*)arg);
    char *ptr;
    while(1)
    {
        executeScript("sudo ./GetPCStatus.sh", &content, MAX_BUFFER_SIZE) ;
        printf("%s", content);

        if(strchr(content,'%') != 0)
        {
            ptr = strchr(content, '\n');
            if(ptr != NULL)
            {
                splitString(content, delimiter, strarr, &arrsize);
                for (int i = 0; i < arrsize; i++)
                {
                    strcpy(buf, strarr[i]);
                    CharCnt = strlen(buf) + 2;
                    buf[CharCnt - 1] = '\n';
                    buf[CharCnt] = '\0';
                    write(fd, buf, CharCnt);
                    msleep(1);
                }
            } 
        }
        else
            sleep(1);

        free(content);
        sleep(1);
    }

    return NULL;
}


void *thread_function2(void *arg) {
    char message[256];
    int GOODCharCnt = 0;
    char *GOOD = "GOOD";
    int fd = *((int*)arg);

    printf("strcpy GOOD\n");
    strcpy(message, GOOD);
    GOODCharCnt = strlen(message) + 2;
    message[GOODCharCnt - 1] = '\n';
    message[GOODCharCnt] = '\0';

    printf("Check File\n");
    while(1)
    {

        write(fd, message, GOODCharCnt);
        msleep(8);
    }
    return NULL;
}

int main(int argc, char **argv) 
{
    int fd;
    pthread_t thread1, thread2;
    int res1, res2;

    printf("uart_start\n");
    if (uart_start(&fd) != 0) {
        return 1;
    }

    res1 = pthread_create(&thread1, NULL, thread_function1, &fd);
    if (res1 != 0) {
        perror("Create thread1 fail");
        return 1;
    }

    res2 = pthread_create(&thread2, NULL, thread_function2, &fd);
    if (res2 != 0) {
        perror("Create thread2 fail");
        return 1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);


    uart_stop(fd);

    return 0;
}
