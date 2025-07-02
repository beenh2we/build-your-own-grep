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
