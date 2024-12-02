# gcc display.c neonate.c iman.c activity.c alias.c reveal.c seek_c.c procl.c comm_log.c command.c main.c -g && gdb ./a.out

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Source files
SRCS =  display.c fgbg.c neonate.c iman.c activity.c alias.c reveal.c seek_c.c procl.c comm_log.c command.c main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Header files (dependencies)
# HEADERS = display.h fab.h input.h list.h log.h hop.h proclore.h reveal.h seek.h redirection.h pipes.h rad.h fg_process.h activities.h iMan.h 

# Default target to build the executable
all: a.out
	chmod +x a.out

# Rule to build the executable from object files
a.out: $(OBJS)
	$(CC) $(CFLAGS) -o a.out $(OBJS)

# Rule to compile each source file into an object file
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f a.out $(OBJS)

# Run the program
run: a.out
	./a.out

.PHONY: all clean run
