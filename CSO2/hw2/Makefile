# File: Makefile

CFLAGS = -O1 -Wall
TARGET = gettimings

all: $(TARGET)

$(TARGET): timings.c
	$(CC) $(CFLAGS) -o $(TARGET) timings.c

clean:
	rm -f $(TARGET)

