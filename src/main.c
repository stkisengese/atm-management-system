#include "header.h"

void mainMenu(struct User u)
{
    int option;
    clearScreen();
    showATMHeader();
    showMainMenu(u);
    
    if (!safeIntInput(&option, "\n\t\t Enter your choice: "))
    {
        showInputErrorMessage();
        mainMenu(u);
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
        // Logout - clear user data and return to login menu
        printf("\n\t\t ✔ Logging out %s...\n", u.name);
        printf("\t\t Returning to login screen.\n");
        sleep(1);
        
        // Clear the user struct
        struct User newUser = {0};
        initMenu(&newUser);
        mainMenu(newUser);
        return;
    case 9:
        showThankYouMessage();
        break;
    default:
        showValidationError("operation", "Please choose a number between 1-9.");
        mainMenu(u);
    }
}

void initMenu(struct User *u)
{
    int r = 0;
    int option;
    char inputPass[50];
    
    while (!r)
    {
        system("clear");
        showATMHeader();
        showLoginMenu(u->name, inputPass);
        
        if (!safeIntInput(&option, "\n\t\t Enter your choice: "))
        {
            showInputErrorMessage();
            continue;
        }
        
        switch (option)
        {
        case 1:
            loginMenu(u->name, inputPass);
            if (getPassword(u))
            {
                if (strcmp(inputPass, u->password) == 0)
                {
                    printf("\n\t\t ✔ Login successful!\n");
                    printf("\t\t Welcome back, %s!\n", u->name);
                    sleep(1);
                    r = 1;
                }
                else
                {
                    showErrorMessage("Wrong password! Please try again.");
                    continue;
                }
            }
            else
            {
                showErrorMessage("Username not found!");
                continue;
            }
            break;
            
        case 2:
        {
            char temp_name[50], temp_pass[50];
            registerMenu(temp_name, temp_pass);
            if (registerUser(temp_name, temp_pass))
            {
                // Populate the user struct with the new user's details
                strcpy(u->name, temp_name);
                strcpy(u->password, temp_pass);
                
                // Get the user ID from database
                if (getPassword(u)) {
                    printf("\n\t\t ✔ Registration successful!\n");
                    printf("\t\t Welcome to the ATM system, %s!\n", u->name);
                    printf("\t\t You're now signed in.\n");
                    sleep(2);
                    r = 1; // Exit the loop to go to main menu
                } else {
                    showErrorMessage("Failed to retrieve user details after registration.");
                    continue;
                }
            }
            else
            {   
                showErrorMessage("Registration failed! Username already exists.");
                continue;
            }
            break;
        }
        
        case 3:
            showThankYouMessage();
            break;
            
        default:
            showValidationError("operation", "Please choose 1, 2, or 3.");
            continue;
        }
    }
}

int main()
{
    struct User u;
    if (initDatabase() != 0)
    {
        printf("Failed to initialize database!\n");
        return 1;
    }
    initMenu(&u);
    mainMenu(u);
    closeDatabase();
    return 0;
}