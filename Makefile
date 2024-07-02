CC := gcc
CFLAGS := -Wall -Wextra -pthread -lrt
DEPFLAGS = -MMD -MP

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)
DEPS := $(OBJECTS:.o=.d)

all: UARTSend

UARTSend: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	rm -f UARTSend $(OBJECTS) $(DEPS)

-include $(DEPS)
