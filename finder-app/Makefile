# Compiler and flags
CC = $(CROSS_COMPILE)gcc

# Source files
SRCS = writer.c

# Object files
OBJS = $(SRCS:.c=.o)

# Output binary
TARGET = writer

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

.PHONY: all clean
# Clean target
clean:
	rm -f $(OBJS) $(TARGET)


