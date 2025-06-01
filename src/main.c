#include "header.h"

void mainMenu(struct User u)
{
    int option;
    system("clear");
    
    printf("\n\t\t\t╔═══════════════════════════╗\n");
    printf("\t\t\t║         ATM SYSTEM        ║\n");
    printf("\t\t\t╚═══════════════════════════╝\n");
    
    printf("\n\t\t-->> Feel free to choose one of the options below <<--\n");
    
    printf("\n\t\t╔═══════════════════════════════════════════╗\n");
    printf("\t\t║  [1] - Create a new account               ║\n");
    printf("\t\t║  [2] - Update account information         ║\n");
    printf("\t\t║  [3] - Check accounts                     ║\n");
    printf("\t\t║  [4] - Check list of owned account        ║\n");
    printf("\t\t║  [5] - Make Transaction                   ║\n");
    printf("\t\t║  [6] - Remove existing account            ║\n");
    printf("\t\t║  [7] - Transfer ownership                 ║\n");
    printf("\t\t║  [8] - Exit                               ║\n");
    printf("\t\t╚═══════════════════════════════════════════╝\n");
    
    if (!safeIntInput(&option, "\n\t\t Enter your choice: "))
    {
        printf("\t\t ✖ Input error. Please try again!\n");
        sleep(2);
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
        printf("\n\t\t Thank you for using our ATM system!\n");
        printf("\t\t Exiting...\n\n");
        closeDatabase();
        exit(0);
        break;
    default:
        printf("\t\t ✖ Invalid operation! Please choose a number between 1-8.\n");
        sleep(2);
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
        printf("\n\t\t\t╔══════════════════════════╗\n");
        printf("\t\t\t║         ATM SYSTEM       ║\n");
        printf("\t\t\t╚══════════════════════════╝\n");
        
        printf("\n\t\t Welcome to the ATM System\n");
        printf("\t\t Please select an option:\n");
        
        printf("\n\t\t╔══════════════════════════════════╗\n");
        printf("\t\t║  [1] - Login to existing account ║\n");
        printf("\t\t║  [2] - Register new user         ║\n");
        printf("\t\t║  [3] - Exit system               ║\n");
        printf("\t\t╚══════════════════════════════════╝\n");
        
        if (!safeIntInput(&option, "\n\t\t Enter your choice: "))
        {
            printf("\t\t ✖ Input error! Please try again.\n");
            sleep(2);
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
                    printf("\n\t\t ✖ Wrong password!\n");
                    sleep(2);
                    continue;
                }
            }
            else
            {
                printf("\n\t\t ✖ Username not found!\n");
                sleep(2);
                continue;
            }
            r = 1;
            break;
            
        case 2:
        {
            char temp_name[50], temp_pass[50];
            registerMenu(temp_name, temp_pass);
            if (registerUser(temp_name, temp_pass))
            {
                printf("\n\t\t ✔ Registration successful!\n");
                printf("\t\t Welcome to the ATM system, %s!\n", temp_name);
                printf("\t\t You're now signed in.\n");
                sleep(2);
            }
            else
            {
                printf("\n\t\t ✖ Registration failed! Username already exists.\n");
                sleep(2);
                initMenu(u);
            }
            return;
        }
        
        case 3:
            printf("\n\t\t Thank you for using our ATM system!\n");
            printf("\t\t Exiting...\n\n");
            closeDatabase();
            exit(0);
            break;
            
        default:
            printf("\t\t ✖ Invalid operation! Please choose 1, 2, or 3.\n");
            sleep(2);
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