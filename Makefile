CC := gcc
CFLAGS := -Wall -Wextra

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)

all: UARTSend

UARTSend: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f UARTSend $(OBJECTS)
