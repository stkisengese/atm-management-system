#include "header.h"

// Helper function to clear input buffer
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// Safe input function that handles spaces and clears buffer
int safeStringInput(char *buffer, int size, const char *prompt)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, size, stdin) != NULL)
    {
        // Remove trailing newline if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
        }
        return 1;
    }
    return 0;
}

// Safe integer input with validation
int safeIntInput(int *value, const char *prompt)
{
    char buffer[50];
    char *endptr;

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        clearInputBuffer(); // Clear any remaining input
        return 0;
    }

    // Remove trailing newline
    buffer[strcspn(buffer, "\n")] = '\0';

    // Check if input is empty
    if (strlen(buffer) == 0)
    {
        return 0;
    }

    // Convert to integer
    long temp = strtol(buffer, &endptr, 10);

    // Check if conversion was successful
    if (*endptr != '\0' || temp < INT_MIN || temp > INT_MAX)
    {
        return 0;
    }

    *value = (int)temp;
    return 1;
}
