#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define DEVICE "/dev/ttyTHS3" // UART Path
#define BAUDRATE B115200        // Baudrate

int uart_start(int *fd);
int uart_reads(int fd, char *buf, int size);
void uart_stop(int fd);