CC = gcc
CFLAGS = -std=gnu99 -Wpedantic -I$(IDIR)

BINDIR = ./bin/
SRCDIR = ./src/
IDIR = ./include

SRCS = $(wildcard $(SRCDIR)*.c)
OBJ = $(patsubst $(SRCDIR)%.c,$(BINDIR)%.o,$(SRCS))

myShell: $(OBJ)
	$(CC) $^ -o $@

$(BINDIR)%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm ./bin/*.o ./myShell