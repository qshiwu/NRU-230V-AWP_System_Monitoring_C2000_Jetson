#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UART.h"
#include "RunScript.h"
#include <pthread.h>
#include <time.h>

#define MAX_BUFFER_SIZE 1024
int Is_Simple = 0;

int read_ini_file(const char *filename, const char *key_to_find, int *value_found) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    char line[MAX_BUFFER_SIZE];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == ';' || line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        char *key = strtok(line, "=");
        char *value_str = strtok(NULL, "\n");

        if (key != NULL && value_str != NULL && strcmp(key, key_to_find) == 0) {
            *value_found = atoi(value_str);
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void msleep(int ms)
{
     usleep(1000 * ms);
}

void sleep_ms(unsigned long milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
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

void *thread_function0(void *arg) {
    int CharCnt = 0;
    char buf[256];
    char *content = NULL;
    int fd = *((int*)arg);
    while(1)
    {
        executeScript("sudo ./GetPCStatus_Simple.sh", &content, MAX_BUFFER_SIZE) ;
        printf("%s", content);

        strcpy(buf, content);
        CharCnt = strlen(buf) + 2;
        buf[CharCnt - 1] = '\n';
        buf[CharCnt] = '\0';
        write(fd, buf, CharCnt);

        free(content);
        sleep(1);
    }

    return NULL;
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
        if(Is_Simple == 0)
            sleep_ms(1);
        else
            sleep_ms(5);
    }
    return NULL;
}

int main(int argc, char **argv) 
{
    int fd;
    pthread_t thread0, thread1, thread2;
    int res0, res1, res2;

    read_ini_file("CANDataFormat.ini", "Is_Simple", &Is_Simple);
    printf("CAN Format : %d\n", Is_Simple);

    printf("uart_start\n");
    if (uart_start(&fd) != 0) {
        return 1;
    }

    pthread_attr_t attr;
    struct sched_param param;
    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_attr_setschedparam(&attr, &param);

    res2 = pthread_create(&thread2, NULL, thread_function2, &fd);
    if (res2 != 0) {
        perror("Create thread2 fail");
        return 1;
    }

    if(Is_Simple == 0)
    {
        //CAN bus don't send anything
    }
    else if(Is_Simple == 1)
    {
        res0 = pthread_create(&thread0, NULL, thread_function0, &fd);
        if (res0 != 0) {
            perror("Create thread0 fail");
            return 1;
        }

        pthread_join(thread0, NULL);
    }
    else
    {
        res1 = pthread_create(&thread1, NULL, thread_function1, &fd);
        if (res1 != 0) {
            perror("Create thread1 fail");
            return 1;
        }
        pthread_join(thread1, NULL);
    }
    
    pthread_join(thread2, NULL);
    uart_stop(fd);

    return 0;
}
