#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

typedef struct
{
    bool ignore_case;    // -i
    bool line_number;    // -n
    bool count_only;     // -c
    bool invert_match;   // -v
    bool use_wildcards;  // -w
    bool use_anchors;    // -a
} grep_options;

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

/**
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

/**
 * Search for a pattern in a file and print matching lines
 */
void search_file(const char *pattern, const char *filename, grep_options opts, bool print_filename)
{
    FILE *file;
    char line[MAX_LINE_LENGTH];
    int line_number = 0;
    int match_count = 0;

    // File or STDIN
    if (strcmp(filename, "stdin") == 0)
    {
        file = stdin;
    }
    else
    {
        file = fopen(filename, "r");
        if (file == NULL)
        {
            fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
            return;
        }
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

    if (file != stdin)
    {
        fclose(file);
    }
}

/**
 * Print usage information
 */
void print_usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s [OPTIONS] PATTERN [FILE...]\n", program_name);
    fprintf(stderr, "Search for PATTERN in each FILE.\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -i       Ignore case distinctions\n");
    fprintf(stderr, "  -n       Print line number with output lines\n");
    fprintf(stderr, "  -c       Print only a count of matching lines per file\n");
    fprintf(stderr, "  -v       Invert the sense of matching, to select non-matching lines\n");
    fprintf(stderr, "  -w       Use wildcard pattern matching (* and ?)\n");
    fprintf(stderr, "  -a       Enable anchor matching (^ for start of line, $ for end of line)\n");
    fprintf(stderr, "  -h       Display this help and exit\n");
}

/**
 * Basic implementation of grep
 * Usage: ./grep <pattern> <file>
 */
int main(int argc, char *argv[])
{
    int opt;
    grep_options options = {false, false, false, false, false, false};

    while ((opt = getopt(argc, argv, "incvwah")) != -1)
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

    // Check if we have enough non-option arguments
    if (optind >= argc)
    {
        fprintf(stderr, "Expected pattern argument\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *pattern = argv[optind++];

    // If no files are specified, read from stdin
    if (optind >= argc)
    {
        search_file(pattern, "stdin", options, false);
    }
    else
    {
        bool print_filename = (argc - optind > 1);

        // Search each file
        for (int i = optind; i < argc; i++)
        {
            if (strcmp(argv[i], "-") == 0)
            {
                // Read from stdin when filename is "-"
                search_file(pattern, "stdin", options, print_filename);
            }
            else
            {
                search_file(pattern, argv[i], options, print_filename);
            }
        }
    }

    return EXIT_SUCCESS;
}
