#include "header.h"

void mainMenu(struct User u)
{
    int option;
    system("clear");
    printf("\n\n\t\t======= ATM =======\n\n");
    printf("\n\t\t-->> Feel free to choose one of the options below <<--\n");
    printf("\n\t\t[1]- Create a new account\n");
    printf("\n\t\t[2]- Update account information\n");
    printf("\n\t\t[3]- Check accounts\n");
    printf("\n\t\t[4]- Check list of owned account\n");
    printf("\n\t\t[5]- Make Transaction\n");
    printf("\n\t\t[6]- Remove existing account\n");
    printf("\n\t\t[7]- Transfer ownership\n");
    printf("\n\t\t[8]- Exit\n");
    scanf("%d", &option);

    switch (option)
    {
    case 1:
        createNewAcc(u);
        break;
    case 2:
        // student TODO : add your **Update account information** function
        // here
        break;
    case 3:
        // student TODO : add your **Check the details of existing accounts** function
        // here
        break;
    case 4:
        checkAllAccounts(u);
        break;
    case 5:
        // student TODO : add your **Make transaction** function
        // here
        break;
    case 6:
        // student TODO : add your **Remove existing account** function
        // here
        break;
    case 7:
        // student TODO : add your **Transfer owner** function
        // here
        break;
    case 8:
        closeDatabase();
        exit(0);
        break;
    default:
        printf("Invalid operation!\n");
        mainMenu(u);
    }
};

void initMenu(struct User *u)
{
    int r = 0;
    int option;
    char inputPass[50];
    system("clear");
    printf("\n\n\t\t======= ATM =======\n");
    printf("\n\t\t-->> Feel free to login / register :\n");
    printf("\n\t\t[1]- login\n");
    printf("\n\t\t[2]- register\n");
    printf("\n\t\t[3]- exit\n");
    while (!r)
    {
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            loginMenu(u->name, inputPass);
            if (getPassword(u))
            {
                if (strcmp(inputPass, u->password) == 0)
                {
                    printf("\n\n✔ Login successful!");
                    sleep(1);
                    r = 1;
                }
                else
                {
                    printf("\n✖ Wrong password!\n");
                    sleep(2);
                    initMenu(u);
                    return;
                }
            }
            else
            {
                printf("\n✖ Username not found!\n");
                sleep(2);
                initMenu(u);
                return;
            }
            r = 1;
            break;
        case 2:
        {
            char temp_name[50], temp_pass[50];
            registerMenu(temp_name, temp_pass);
            if (registerUser(temp_name, temp_pass))
            {
                printf("\n\nRegistration successful! Please login.");
                sleep(2);
                initMenu(u);
            }
            else
            {
                printf("\n\nRegistration failed! Username might already exist.");
                sleep(2);
                initMenu(u);
            }
            return;
        }
        case 3:
            closeDatabase();
            exit(0);
            break;
        default:
            printf("Insert a valid operation!\n");
        }
    }
};

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
