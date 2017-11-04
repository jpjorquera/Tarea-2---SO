TARGET = prog
CC = g++
CFLAGS = -Wall

.PHONY: default all clean

default: $(TARGET)

all: default

OBJECTS = src/functions.cpp src/mazo.cpp src/main.cpp
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $@

run:
	./$(TARGET)

clean:
	-rm -f *.o
	-rm -f $(TARGET)