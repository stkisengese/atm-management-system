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

// Helper function to display interest information
void displayInterestInfo(const char* accountType, double balance, const char* depositDate)
{
    printf("\n========== Interest Information ==========\n");
    
    if (strcmp(accountType, "current") == 0)
    {
        printf("You will not get interests because the account is of type current\n");
    }
    else
    {
        double interestRate = 0.0;
        if (strcmp(accountType, "saving") == 0)
            interestRate = 0.07; // 7%
        else if (strcmp(accountType, "fixed01") == 0)
            interestRate = 0.04; // 4%
        else if (strcmp(accountType, "fixed02") == 0)
            interestRate = 0.05; // 5%
        else if (strcmp(accountType, "fixed03") == 0)
            interestRate = 0.08; // 8%

        double monthlyInterest = (balance * interestRate) / 12;
        
        // Extract day from deposit date (format: mm/dd/yyyy)
        int month, day, year;
        sscanf(depositDate, "%d/%d/%d", &month, &day, &year);
        
        printf("Interest rate: %.1f%%\n", interestRate * 100);
        printf("You will get $%.2f as interest on day %d of every month\n", monthlyInterest, day);
    }
}