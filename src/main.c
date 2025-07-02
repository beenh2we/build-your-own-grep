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
