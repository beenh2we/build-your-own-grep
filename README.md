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

## Stage 2: Multiple File Support and Basic Options

Let's enhance our program to support multiple input files and add some common grep options.

### Adding Multiple File Support

**src/main.c**:

```c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

typedef struct
{
    bool ignore_case;   // -i
    bool line_number;   // -n
    bool count_only;    // -c
    bool invert_match;  // -v
} grep_options;

void search_file(const char *pattern, const char *filename, bool print_filename)
{
    FILE *file;
    char line[MAX_LINE_LENGTH];

    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return;
    }

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        if (strstr(line, pattern) != NULL)
        {
            if (print_filename)
            {
                printf("%s:%s", filename, line);
            }
            else
            {
                printf("%s", line);
            }

            if (line[0] != '\0' && line[strlen(line) - 1] != '\n')
            {
                printf("\n");
            }
        }
    }

    fclose(file);
}

/**
 * Basic implementation of grep
 * Usage: ./grep <pattern> <file>
 */
int main(int argc, char const *argv[])
{
    // Check for minimum required arguments
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <pattern> <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *pattern = argv[1];
    bool print_filename = (argc > 3);

    for (int i = 2; i < argc; i++)
    {
        search_file(pattern, argv[i], print_filename);
    }

    return EXIT_SUCCESS;
}
```

### Adding Basic Options

Now let's add support for some basic grep options:

**src/main.c**:

```c
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

typedef struct
{
    bool ignore_case;   // -i
    bool line_number;   // -n
    bool count_only;    // -c
    bool invert_match;  // -v
} grep_options;

/**
 * Check if line matches the pattern based on the provided options
 */
bool line_matches(const char *line, const char *pattern, grep_options opts)
{
    bool match = false;
    if (opts.ignore_case)
    {
        // Case-insentive search
        char line_lower[MAX_LINE_LENGTH];
        char pattern_lower[MAX_LINE_LENGTH];

        size_t i;
        for (i = 0; line[i] && i < MAX_LINE_LENGTH - 1; i++)
        {
            line_lower[i] = tolower(line[i]);
        }
        line_lower[i] = '\0';

        for (i = 0; pattern[i] && i < MAX_LINE_LENGTH - 1; i++)
        {
            pattern_lower[i] = tolower(pattern[i]);
        }
        pattern_lower[i] = '\0';

        match = strstr(line_lower, pattern_lower) != NULL;
    }
    else
    {
        // Case-sensitive search
        match = strstr(line, pattern) != NULL;
    }

    // Apply invert_match option if needed
    return opts.invert_match ? !match : match;
}

/**
 * Search for a pattern in a file and print matching lines
 */
void search_file(const char *pattern, const char *filename, grep_options opts, bool print_filename)
{
    FILE *file;
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int match_count = 0;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return;
    }

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        line_number++;

        if (line_matches(line, pattern, opts))
        {
            match_count++;

            if (!opts.count_only)
            {
                if (print_filename)
                {
                    printf("%s:", filename);
                }

                if (opts.line_number)
                {
                    printf("%d:", line_number);
                }

                printf("%s", line);

                // Add newline if not present
                if (line[0] != '\0' && line[strlen(line) - 1] != '\n')
                {
                    printf("\n");
                }
            }
        }
    }

    if (opts.count_only)
    {
        if (print_filename)
        {
            printf("%s:", filename);
        }
        printf("%d\n", match_count);
    }

    fclose(file);
}

/**
 * Print usage information
 */
void print_usage(const char *program_name)
{
    fprintf(stderr, "Usaeg: %s [OPTIONS] PATTERN [FILE...]\n", program_name);
    fprintf(stderr, "Search for PATTERN in each FILE.\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -i         Ignore case distinctions\n");
    fprintf(stderr, "  -n         Print line number with output lines\n");
    fprintf(stderr, "  -c         Print only a count of matching lines per file\n");
    fprintf(stderr, "  -v         Invert the sense of matching, to select non-matching lines\n");
    fprintf(stderr, "  -h         Display this help and exit\n");
}

/**
 * Basic implementation of grep
 * Usage: ./grep <pattern> <file>
 */
int main(int argc, char *argv[])
{
    int opt;
    grep_options options = {false, false, false, false};

    while ((opt = getopt(argc, argv, "incvh")) != -1)
    {
        switch (opt)
        {
        case 'i':
            options.ignore_case = true;
            break;
        case 'n':
            options.line_number = true;
            break;
        case 'c':
            options.count_only = true;
            break;
        case 'v':
            options.invert_match = true;
            break;
        case 'h':
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        default:
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // Check if we have enough non-option arguments
    if (optind >= argc)
    {
        fprintf(stderr, "Expected pattern argument\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *pattern = argv[optind++];

    // No files specified
    if (optind >= argc)
    {
        fprintf(stderr, "No input files specified\n");
        return EXIT_FAILURE;
    }

    bool print_filename = (argc - optind > 1);

    // Search each file
    for (int i = optind; i < argc; i++)
    {
        search_file(pattern, argv[i], options, print_filename);
    }

    return EXIT_SUCCESS;
}
```

### Standard Input Support

Let's add support for reading from standard input when no files are specified or when `-` is used as a filename:

```c
// In main() function, replace the "If no files are specified" section with:

// If no files are specified, read from stdin
if (optind >= argc) {
    search_file(pattern, "stdin", options, false);
} else {
    bool print_filename = (argc - optind > 1);

    // Search each file
    for (int i = optind; i < argc; i++) {
        if (strcmp(argv[i], "-") == 0) {
            // Read from stdin when filename is "-"
            search_file(pattern, "stdin", options, print_filename);
        } else {
            search_file(pattern, argv[i], options, print_filename);
        }
    }
}
```

And modify the search_file() function:

```c
void search_file(const char *pattern, const char *filename, grep_options opts, bool print_filename) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int match_count = 0;

    // Open file or use stdin
    if (strcmp(filename, "stdin") == 0) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
        if (file == NULL) {
            fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
            return;
        }
    }

    // Rest of the function remains the same

    // Close the file if it's not stdin
    if (file != stdin) {
        fclose(file);
    }
}
```

### Building and Testing

Compile and test the program:

```bash
make

# Test with multiple files
./build/grep "world" test.txt test.txt

# Test with options
./build/grep -i "WORLD" test.txt
./build/grep -n "world" test.txt
./build/grep -c "world" test.txt
./build/grep -v "world" test.txt

# Test with stdin
cat test.txt | ./build/grep "world" -
```

### Code Explanation

In this stage, we've:

1. Refactored the code to support multiple files
2. Added a `grep_options` struct to hold command-line options
3. Implemented option parsing with `getopt()`
4. Added case-insensitive searching with `tolower()`
5. Added line numbering and match counting
6. Implemented inverted matching
7. Added support for reading from standard input

We've also improved the organization by:

- Creating a separate function for file searching
- Creating a function to check if a line matches
- Adding a usage function for help text

---
