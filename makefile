# Compiler and Flags
CC = gcc
CFLAGS = -g -Wall 

# Output executable name
TARGET = n64_emulator

# List of source files
SOURCES = main.c n64.c pipeline.c rom_loading.c debug_sys.c control.c cp0.c cpu.c mmu.c memory.c rsp.c rcp.c parallel_interface.c serial_interface.c video_interface.c

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