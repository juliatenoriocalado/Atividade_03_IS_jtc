CC = gcc
CFLAGS = -Wall -Wextra

TARGET = scheduler
SRC = escalonamento.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
		rm -f $(TARGET)

.PHONY: all clean