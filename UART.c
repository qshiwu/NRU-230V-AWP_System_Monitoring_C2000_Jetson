#include <stdio.h>
#include <stdlib.h>
#include "UART.h"


// UART Open
int uart_start(int *fd) {
    struct termios tty;

    // Open UART device
    *fd = open(DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (*fd < 0) {
        perror("Error opening device");
        return 1;
    }

    // Set UART attributes
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(*fd, &tty) != 0) {
        perror("Error from tcgetattr");
        return 1;
    }


    //printf(BAUDRATE);
    cfsetospeed(&tty, BAUDRATE);
    cfsetispeed(&tty, BAUDRATE);

    tty.c_cflag |= (CLOCAL | CREAD);    
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 // 8 bits per byte
    tty.c_cflag &= ~PARENB;              // Disable parity
    tty.c_cflag &= ~CSTOPB;              // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;             // Disable hardware flow control

    // Set non-canonical mode
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Disable canonical mode, echo, and signals
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);          // Disable input/output flow control

    // Set timeout and minimum bytes
    tty.c_cc[VMIN] = 1;  // Minimum bytes to read
    tty.c_cc[VTIME] = 0; // Timeout (0 means no timeout)

    // Apply attributes to device
    if (tcsetattr(*fd, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        return 1;
    }

    return 0;
}

// UART Read function
int uart_reads(int fd, char *buf, int size) {
    // Read data
    int n = read(fd, buf, size);
    if (n < 0) {
        perror("Error reading from UART");
    } else {
        buf[n] = '\0';
    }
    return n;
}

// UART Close function
void uart_stop(int fd) {
    // Close UART device
    close(fd);
}

