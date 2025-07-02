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

## Stage 3: Pattern Matching

Now let's enhance our pattern matching capabilities to support basic wildcard patterns and regular expressions.

### Simple Pattern Matching

First, let's implement a simple pattern matching function that supports basic wildcards:

```c
bool match_from_current_position(const char *line, const char *pattern)
{
    // End of pattern reached, return true
    if (*pattern == '\0') return true;

    // Handle * wildcard (matches zero or more characters)
    if (*pattern == '*')
    {
        // Skip the wildcard
        pattern++;

        // Try to match the rest of the pattern with different amounts of text
        while (*line)
        {
            if (match_from_current_position(line, pattern))
            {
                return true;
            }
            line++;
        }

        // Check if the remaining pattern matches empty text
        return match_from_current_position(line, pattern);
    }

    // Handle ? wildcard (matches exactly one character)
    if (*pattern == '?' && *line != '\0')
    {
        return match_from_current_position(line + 1, pattern + 1);
    }

    // If the current characters match, check the rest of the string
    if (*pattern == *line)
    {
        return match_from_current_position(line + 1, pattern + 1);
    }

    return false;
}

/**
 * Simple pattern matching function that supports * and ? wildcards
 * * matches zero or more characters
 * ? matches exactly one character
 */
bool match_pattern(const char *line, const char *pattern)
{
    if (*pattern == '\0') return true;

    while (*line)
    {
        if (match_from_current_position(line, pattern)) return true;

        line++;
    }

    return false;
}
```

### Adding Support for Line Anchors

Let's add support for the `^` and `$` anchors, which match the beginning and end of a line:

```c
/**
 * Check if a line matches a pattern with anchors (^ and $)
 * ^ matches the start of a line (like "^abc" matches "abc..." but not "...abc")
 * $ matches the end of a line (like "abc$" matches "...abc" but not "abc...")
 */
bool match_with_anchors(const char *line, const char *pattern)
{
    size_t pattern_len = strlen(pattern);
    size_t line_len = strlen(line);

    // Handle ^ anchor (start of line)
    if (pattern[0] == '^')
    {
        // Check if the line starts with the pattern (minus the ^)
        const char *subpattern = pattern + 1;
        size_t subpattern_len = pattern_len - 1;

        // Check if the line starts with the subpattern
        if (line_len < subpattern_len)
        {
            return false;
        }

        for (size_t i = 0; i < subpattern_len; i++)
        {
            if (subpattern[i] != line[i])
            {
                return false;
            }
        }
        return true;
    }

    // Handle $ anchor (end of line)
    if (pattern_len > 0 && pattern[pattern_len - 1] == '$')
    {
        // Check if the line ends with the pattern (minus the $)
        size_t subpattern_len = pattern_len - 1;

        // If the line has a trailing newline, ignore it for comparison
        size_t effective_line_len = line_len;
        if (line_len > 0 && line[line_len - 1] == '\n')
        {
            effective_line_len--;
        }

        if (effective_line_len < subpattern_len)
        {
            return false;
        }

        // Check if the line ends with the subpattern
        const char *line_end = line + effective_line_len - subpattern_len;
        for (size_t i = 0; i < subpattern_len; i++)
        {
            if (pattern[i] != line_end[i])
            {
                return false;
            }
        }

        return true;
    }

    // If no anchors, just check if the pattern appears anywhere in the line
    return strstr(line, pattern) != NULL;
}
```

### Updating Line Matching Function

Now let's update our line matching function to use these pattern matching capabilities:

```c
typedef struct {
    bool ignore_case;   // -i option
    bool line_number;   // -n option
    bool count_only;    // -c option
    bool invert_match;  // -v option
    bool use_wildcards; // -w option (new)
    bool use_anchors;   // -a option (new)
} grep_options;

**
 * Check if line matches the pattern based on the provided options
 */
bool line_matches(const char *line, const char *pattern, grep_options opts)
{
    bool match = false;

    // Case-insentive search
    char line_copy[MAX_LINE_LENGTH];
    char pattern_copy[MAX_LINE_LENGTH];

    if (opts.ignore_case)
    {
        size_t i;
        for (i = 0; line[i] && i < MAX_LINE_LENGTH - 1; i++)
        {
            line_copy[i] = tolower(line[i]);
        }
        line_copy[i] = '\0';

        for (i = 0; pattern[i] && i < MAX_LINE_LENGTH - 1; i++)
        {
            pattern_copy[i] = tolower(pattern[i]);
        }
        pattern_copy[i] = '\0';
    }
    else
    {
        strcpy(line_copy, line);
        strcpy(pattern_copy, pattern);
    }

    if (opts.use_anchors)
    {
        match = match_with_anchors(line_copy, pattern_copy);
    }
    else if (opts.use_wildcards)
    {
        match = match_pattern(line_copy, pattern_copy);
    }
    else
    {
        match = strstr(line_copy, pattern_copy) != NULL;
    }

    // Apply invert_match option if needed
    return opts.invert_match ? !match : match;
}
```

### Adding New Command-Line Options

Now let's add these new options to our main function:

```c
int main(int argc, char *argv[]) {
    int opt;
    grep_options options = {false, false, false, false, false, false};

    // Parse command-line options
    while ((opt = getopt(argc, argv, "incvwah")) != -1) {
        switch (opt) {
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
            case 'w':
                options.use_wildcards = true;
                break;
            case 'a':
                options.use_anchors = true;
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Rest of the function remains the same
}
```

Don't forget to update the usage function:

```c
void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [OPTIONS] PATTERN [FILE...]\n", program_name);
    fprintf(stderr, "Search for PATTERN in each FILE.\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -i         Ignore case distinctions\n");
    fprintf(stderr, "  -n         Print line number with output lines\n");
    fprintf(stderr, "  -c         Print only a count of matching lines per file\n");
    fprintf(stderr, "  -v         Invert the sense of matching, to select non-matching lines\n");
    fprintf(stderr, "  -w         Use wildcard pattern matching (* and ?)\n");
    fprintf(stderr, "  -a         Enable anchor matching (^ for start of line, $ for end of line)\n");
    fprintf(stderr, "  -h         Display this help and exit\n");
}
```

### Building and Testing

Compile and test the program:

```bash
make

# Test wildcard matching
./build/grep -w "w*d" test.txt

# Test anchor matching
./build/grep -a "^Hello" test.txt
./build/grep -a "world!$" test.txt

# Combine options
./build/grep -i -a "^hello" test.txt
```

### Code Explanation

In this stage, we've added:

1. A recursive pattern matching function for wildcards `*` and `?`
2. Support for line anchors `^` and `$`
3. Command-line options to enable these features
4. Integration of these features with the existing code

The pattern matching algorithm is a simple recursive implementation that:

- Handles the `*` wildcard by trying different amounts of text
- Handles the `?` wildcard by skipping exactly one character
- Matches character by character for normal text
- Supports special anchors for line beginning and ending

This provides a solid foundation for pattern matching, though it's not as comprehensive as full regular expressions.

---
