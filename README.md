# Building Your Own `grep` in C: A Step-by-Step Guide

## Project Overview

This guide will walk you through implementing a `grep`-like utility in C, starting from basic functionality and gradually building up to a more complete solution. This approach will help you solidify your C syntax understanding, learn about file I/O, memory management, and build systems.

### What is `grep`?

`grep` (Global Regular Expression Print) is a command-line utility that searches text files for lines containing a specific pattern. It's one of the most commonly used tools in Unix/Linux systems.

### Learning Objectives

- Master C file I/O operations
- Practice memory management
- Understand string manipulation in C
- Implement command-line argument parsing
- Create modular and maintainable code
- Use build systems (Make and CMake)
- Implement pattern matching algorithms
- Handle errors properly in C

## Implementation Roadmap

We'll build our `grep` implementation in stages:

1. **Stage 1**: Basic string search in a single file
2. **Stage 2**: Multiple file support and command-line options
3. **Stage 3**: Pattern matching and regular expressions
4. **Stage 4**: Context lines and formatting
5. **Stage 5**: Recursive search and binary file handling
6. **Stage 6**: Performance optimizations
7. **Stage 7**: Project organization with CMake

---

## Stage 1: Basic Implementation

Let's start with a minimal working version that can search for a fixed string pattern in a single file.

### Project Setup

First, let's create a simple project structure:

```plaintext
.
├── Makefile
├── README.md
└── src
    └── main.c
```

### Basic Implementation

Let's write our first version:

**src/main.c**:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

/**
 * Basic implementation of grep
 * Usage: ./grep <pattern> <file>
 */
int main(int argc, char const *argv[])
{
    FILE *file;
    char line[MAX_LINE_LENGTH];

    // Validate command-line arguments
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <pattern> <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *pattern = argv[1];
    const char *filename = argv[2];

    // Open the file
    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return EXIT_FAILURE;
    }

    // Read the file line by line
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        // Check if the line contains the pattern
        if (strstr(line, pattern) != NULL)
        {
            printf("%s", line);
        }
    }

    // Close the file
    fclose(file);

    return EXIT_SUCCESS;
}
```

**Makefile**:

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c17

BUILD_DIR = build

TARGET = grep
TARGET_PATH = $(BUILD_DIR)/$(TARGET)

all: $(TARGET_PATH)

$(BUILD_DIR):
	mkdir -p $@

$(TARGET_PATH): src/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

run: $(TARGET_PATH)
	./$(TARGET_PATH)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run
```

### Building and Testing

Compile the program:

```bash
make
```

Test the program:

```bash
# Create a test file
echo "Hello, world!" > test.txt
echo "This is a test file." >> test.txt
echo "Goodbye, world!" >> test.txt

# Run your grep implementation
./build/grep "world" test.txt
```

You should see:

```
Hello, world!
Goodbye, world!
```

### Code Explanation

This basic implementation:

1. Defines a maximum line length to limit memory usage
2. Validates command-line arguments
3. Opens the specified file for reading
4. Reads the file line by line using `fgets()`
5. Uses `strstr()` to check if each line contains the pattern
6. Prints matching lines to standard output
7. Properly closes the file when done

The program uses:

- `FILE*` and `fopen()` for file operations
- `fgets()` for line-based reading
- `strstr()` for substring matching
- `fprintf()` for error messages
- Proper exit codes with `EXIT_SUCCESS` and `EXIT_FAILURE`

See details explanation: [A Comprehensive Guide to a Basic `grep` Implementation in C](./docs/stage1-basic-string-search.md)

#### Technical Points

1. **Buffered I/O**: The C standard I/O functions (`fgets()`, `printf()`, etc.) use internal buffering for efficiency.

2. **Text Mode vs. Binary Mode**: The file is opened in text mode (`"r"`), which handles platform-specific line endings automatically.

3. **Stream-Based I/O**: The program uses stream-based I/O (`FILE*`) rather than low-level file descriptors, providing buffering and higher-level abstractions.

4. **Command-Line Interface**: The program demonstrates a standard UNIX-style command-line interface with positional arguments.

5. **Memory Safety**: The implementation avoids common memory safety issues by using bounds-checked functions and fixed-size buffers appropriately.

6. **Standard Streams**: The program uses both `stdout` (via `printf()`) for normal output and `stderr` (via `fprintf()`) for error messages.

7. **Substring Searching**: The program demonstrates basic string manipulation with the `strstr()` function for pattern matching.

8. **Line-Oriented Processing**: The implementation processes files line by line, a common pattern in text processing utilities.

9. **Defensive Programming**: The code includes checks for error conditions and provides appropriate feedback when errors occur.

10. **Exit Code Conventions**: The program follows standard conventions for exit codes, using `EXIT_SUCCESS` and `EXIT_FAILURE`.

---
