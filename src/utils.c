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
void displayInterestInfo(const char *accountType, double balance, const char *depositDate)
{
    printf("\n═════════════════════ Interest Information ════════════════════\n");

    if (strcmp(accountType, "current") == 0)
    {
        printf("You will not get interests because the account is of type current\n");
        return;
    }

    double interestRate = 0.0;
    int maturityYears = 0;

    // Set interest rates and maturity periods
    if (strcmp(accountType, "saving") == 0)
    {
        interestRate = 0.07; // 7% annual
    }
    else if (strcmp(accountType, "fixed01") == 0)
    {
        interestRate = 0.04; // 4% annual
        maturityYears = 1;
    }
    else if (strcmp(accountType, "fixed02") == 0)
    {
        interestRate = 0.05; // 5% annual
        maturityYears = 2;
    }
    else if (strcmp(accountType, "fixed03") == 0)
    {
        interestRate = 0.08; // 8% annual
        maturityYears = 3;
    }

    // Extract date components from deposit date (format: mm/dd/yyyy)
    int month, day, year;
    sscanf(depositDate, "%d/%d/%d", &month, &day, &year);
    printf("Interest rate: %.1f%% per annum\n", interestRate * 100);
    if (strcmp(accountType, "saving") == 0)
    {
        // For saving accounts: monthly interest
        double monthlyInterest = (balance * interestRate) / 12;
        printf("You will get $%.2f as interest on day %d of every month\n",
               monthlyInterest, day);
    }
    else
    {
        // For fixed accounts: SIMPLE INTEREST (Simple Interest = Principal × Rate × Time)
        double totalInterest = balance * interestRate * maturityYears;
        double maturityAmount = balance + totalInterest;
        int maturityYear = year + maturityYears;

        printf("Maturity Period: %d year(s)\n", maturityYears);
        printf("Maturity Date: %02d/%02d/%04d\n", month, day, maturityYear);
        printf("You will get $%.2f as interest on %02d/%02d/%04d\n",
               totalInterest, month, day, maturityYear);
        printf("Total Amount at Maturity: $%.2f\n", maturityAmount);
    }

    printf("═══════════════════════════════════════════════════════════════\n");
}

// Helper function to check if account is fixed type
int isFixedAccount(const char *accountType)
{
    return (strcmp(accountType, "fixed01") == 0 ||
            strcmp(accountType, "fixed02") == 0 ||
            strcmp(accountType, "fixed03") == 0);
}

int authenticateUser(struct User *u, char *inputPass)
{
    if (!getPassword(u))
    {
        showErrorMessage("Username not found!");
        return 0;
    }

    if (strcmp(inputPass, u->password) != 0)
    {
        showErrorMessage("Wrong password! Please try again.");
        return 0;
    }

    printf("\n\t\t ✔ Login successful!\n");
    printf("\t\t Welcome back, %s!\n", u->name);
    sleep(1);
    return 1;
}

int registerNewUser(struct User *u, char *temp_name, char *temp_pass)
{
    if (!registerUser(temp_name, temp_pass))
    {
        showErrorMessage("Registration failed! Username already exists.");
        return 0;
    }

    // Populate user struct with new user details
    strcpy(u->name, temp_name);
    strcpy(u->password, temp_pass);

    if (!getPassword(u))
    {
        showErrorMessage("Failed to retrieve user details after registration.");
        return 0;
    }

    printf("\n\t\t ✔ Registration successful!\n");
    printf("\t\t Welcome to the ATM system, %s!\n", u->name);
    printf("\t\t You're now signed in.\n");
    sleep(2);
    return 1;
}

// Logout helper function
void handleLogout(struct User u)
{
    printf("\n\t\t ✔ Logging out %s...\n", u.name);
    printf("\t\t Returning to login screen.\n");
    sleep(1);

    struct User newUser = {0};
    initMenu(&newUser);
    mainMenu(newUser);
}

// Utility function to get valid menu input
int getMenuChoice(const char *prompt, int min, int max)
{
    int option;

    if (!safeIntInput(&option, prompt))
    {
        showInputErrorMessage();
        return -1; // Invalid input
    }

    if (option < min || option > max)
    {
        char rangeMsg[100];
        snprintf(rangeMsg, sizeof(rangeMsg), "Please choose a number between %d-%d.", min, max);
        showValidationError("operation", rangeMsg);
        return -1; // Out of range
    }

    return option;
}