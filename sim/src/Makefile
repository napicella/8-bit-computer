.DEFAULT_GOAL := run

CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99 -I../modules/vrEmu6502/src -I../modules/vrEmu6522/src
LDFLAGS = -L../modules/vrEmu6502/bin/src -lvrEmu6502 -L../modules/vrEmu6522/bin/src -lvrEmu6522
SRCDIR = .
OBJDIR = bin
TARGET = $(OBJDIR)/my_program
LD_PATH = ../modules/vrEmu6502/bin/src:../modules/vrEmu6522/bin/src:$(LD_LIBRARY_PATH)

# List of source files
SRCS = $(wildcard $(SRCDIR)/*.c)

# List of object files
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Main rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Rule to compile each source file into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJDIR)

build: $(TARGET)

run: $(TARGET)
	LD_LIBRARY_PATH=$(LD_PATH) $(TARGET) > /dev/null

run-log: $(TARGET)
	LD_LIBRARY_PATH=$(LD_PATH) $(TARGET)

# Phony target to prevent conflicts with files named "clean"
.PHONY: clean
