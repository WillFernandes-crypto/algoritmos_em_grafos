CC = gcc
CFLAGS = -Wall -g
SRC = main.c graph.c region.c report.c utils.c wildfire_management.c
OBJ = $(SRC:.c=.o)
EXEC = queimadas

all: $(EXEC)

$(EXEC): $(OBJ)
    $(CC) $(CFLAGS) -o $@ $^

%.o: %.c
    $(CC) $(CFLAGS) -c $<

clean:
    rm -f *.o $(EXEC)