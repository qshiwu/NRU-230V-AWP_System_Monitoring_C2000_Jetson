#include <stdio.h>
#include <stdlib.h>
#include "UART.h"

void msleep(int ms)
{
     usleep(1000 * ms);
}

int main(int argc, char **argv) 
{
    int fd;
    char buf[256];
    //int n;
    char message[256];
    int CharCnt = 0;

    // Open UART
    if (uart_start(&fd) != 0) {
        return 1;
    }

    strcpy(message, argv[1]);
    CharCnt = strlen(message) + 1;
    message[CharCnt] = '\0';
    write(fd, message, CharCnt);

    msleep(50);  //delay 10ms

    //n = uart_reads(fd, buf, sizeof(buf));
    //if (n > 0) {
    //    printf("Received: %s\n", buf);
    //}


    // close UART
    uart_stop(fd);

    return 0;
}
