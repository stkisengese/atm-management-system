#include "header.h"

// Constants for menu bounds
#define MAIN_MENU_MIN 1
#define MAIN_MENU_MAX 9
#define LOGIN_MENU_MIN 1
#define LOGIN_MENU_MAX 3

void mainMenu(struct User u)
{
    int option;

    clearScreen();
    showATMHeader();
    showMainMenu(u);

    option = getMenuChoice("\n\t\t Enter your choice: ", MAIN_MENU_MIN, MAIN_MENU_MAX);
    if (option == -1)
    {
        mainMenu(u); // Retry on invalid input
        return;
    }

    switch (option)
    {
    case 1:
        createNewAcc(u);
        break;
    case 2:
        updateAccountInfo(u);
        break;
    case 3:
        checkAccountDetails(u);
        break;
    case 4:
        checkAllAccounts(u);
        break;
    case 5:
        makeTransaction(u);
        break;
    case 6:
        removeAccount(u);
        break;
    case 7:
        transferOwnership(u);
        break;
    case 8:
        handleLogout(u);
        break;
    case 9:
        showThankYouMessage();
        break;
    }
}

void initMenu(struct User *u)
{
    int authenticated = 0;
    int option;
    char inputPass[50];

    while (!authenticated)
    {
        system("clear");
        showATMHeader();
        showLoginMenu(u->name, inputPass);

        option = getMenuChoice("\n\t\t Enter your choice: ", LOGIN_MENU_MIN, LOGIN_MENU_MAX);
        if (option == -1)
        {
            continue; // Retry on invalid input
        }

        switch (option)
        {
        case 1: // Login
            loginMenu(u->name, inputPass);
            authenticated = authenticateUser(u, inputPass);
            break;

        case 2:
        { // Register
            char temp_name[50], temp_pass[50];
            registerMenu(temp_name, temp_pass);
            authenticated = registerNewUser(u, temp_name, temp_pass);
            break;
        }

        case 3: // Exit
            showThankYouMessage();
            exit(0); // Exit the program entirely
            break;
        }
    }
}

int main()
{
    struct User u = {0}; // Initialize to zero

    if (initDatabase() != 0)
    {
        fprintf(stderr, "Failed to initialize database!\n");
        return 1;
    }

    initMenu(&u);
    mainMenu(u);

    closeDatabase();
    return 0;
}