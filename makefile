# Compiler and Flags
CC = gcc
CFLAGS = -Wall -g

# Output executable name
TARGET = n64_emulator

# List of source files
SOURCES = main.c pipeline.c rom_loading.c debug_sys.c instruction_control.c memory.c cp0.c cpu.c mmu.c

# Automatically create object files from source files
OBJECTS = $(SOURCES:.c=.o)

# Default rule (this will be run when you type 'make' without any arguments)
all: $(TARGET)

# Linking the object files to create the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Rule to compile each source file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the generated object files and executable
clean:
	rm -f $(OBJECTS) $(TARGET)