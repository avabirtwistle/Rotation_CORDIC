CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11
LDLIBS = -lm

TARGET = cordic_tb

SOURCES = cordic_tb.c cordic_R_fixed_point.c
OBJECTS = $(SOURCES:.c=.o)

HEADERS = cordic_R_fixed_point.h table.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDLIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all run clean