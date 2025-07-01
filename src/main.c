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
