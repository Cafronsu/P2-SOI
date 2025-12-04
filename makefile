CC=gcc
CFLAGS=-c -g -Wall -std=c17

SOURCES=nivel6.c
PROGRAMS=nivel6
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

nivel6: nivel6.o
	$(CC) $@.o -o $@ $(LIBRARIES)

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ *.tmp $(PROGRAMS)
