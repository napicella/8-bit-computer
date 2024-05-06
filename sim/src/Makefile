.DEFAULT_GOAL := run

CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99 -I../modules/vrEmu6502/src
LDFLAGS = -L../modules/vrEmu6502/bin/src -lvrEmu6502
SRCDIR = .
OBJDIR = bin
TARGET = $(OBJDIR)/my_program

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

run: $(TARGET)
	LD_LIBRARY_PATH=../modules/vrEmu6502/bin/src:$(LD_LIBRARY_PATH) $(TARGET) > /dev/null

# Phony target to prevent conflicts with files named "clean"
.PHONY: clean
