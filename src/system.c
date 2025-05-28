#include "header.h"

void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
    int option;
    if (notGood == 0)
    {
        system("clear");
        printf("\n✖ Record not found!!\n");
    invalid:
        if (!safeIntInput(&option, "\nEnter 0 to try again, 1 to return to main menu and 2 to exit: "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            goto invalid;
        }

        if (option == 0)
            f(u);
        else if (option == 1)
            mainMenu(u);
        else if (option == 2)
        {
            closeDatabase();
            exit(0);
        }
        else
        {
            printf("✖ Invalid operation! Please choose 0, 1, or 2.\n");
            sleep(2);
            goto invalid;
        }
    }
    else
    {
        if (!safeIntInput(&option, "\nEnter 1 to go to the main menu and 0 to exit: "))
        {
            printf("✖ Invalid input! Returning to main menu.\n");
            sleep(2);
            option = 1;
        }
    }

    if (option == 1)
    {
        system("clear");
        mainMenu(u);
    }
    else
    {
        system("clear");
        printf("Thank you for using our ATM system!\n");
        closeDatabase();
        exit(0);
    }
}

void success(struct User u)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    if (!safeIntInput(&option, "Enter 1 to go to the main menu and 0 to exit: "))
    {
        printf("✖ Invalid input! Please try again.\n");
        sleep(2);
        goto invalid;
    }

    system("clear");
    if (option == 1)
    {
        mainMenu(u);
    }
    else if (option == 0)
    {
        printf("Thank you for using our ATM system!\n");
        closeDatabase();
        exit(0);
    }
    else
    {
        printf("✖ Invalid operation! Please choose 1 or 0.\n");
        sleep(2);
        goto invalid;
    }
}

void createNewAcc(struct User u)
{
    struct Record r;
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;
    char input[100];

    system("clear");
    printf("\t\t\t===== New record =====\n");

    // Validate date input
    int isValidDate = 0;
    do
    {
        int month, day, year;
        if (!safeIntInput(&month, "\nEnter today's month (mm): "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        if (!safeIntInput(&day, "Enter today's day (dd): "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        if (!safeIntInput(&year, "Enter today's year (yyyy): "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        r.deposit.month = month;
        r.deposit.day = day;
        r.deposit.year = year;

        isValidDate = validateDate(r.deposit.month, r.deposit.day, r.deposit.year);
        if (!isValidDate)
            sleep(2);
    } while (!isValidDate);

    // Validate account number input
    do
    {
        if (!safeStringInput(input, sizeof(input), "\nEnter the account number: "))
        {
            printf("✖ Input error! Please try again.\n");
            sleep(2);
            continue;
        }

        if (!validateAccountNumber(input))
        {
            sleep(2);
            continue;
        }

        // convert to integer
        r.accountNbr = atoi(input);

        // Check if account already exists
        if (accountExists(r.accountNbr))
        {
            printf("✖ This Account is already in use. Please choose a different one.\n");
            sleep(2);
            continue;
        }

        break;
    } while (1);

    int isValid = 0;
    // Validate country input
    do
    {
        if (!safeStringInput(r.country, sizeof(r.country), "\nEnter the country: "))
        {
            printf("✖ Input error! Please try again.\n");
            sleep(2);
            continue;
        }

        isValid = validateName(r.country);
        if (!isValid)
            sleep(2);
    } while (!isValid);

    // Validate phone number input
    do
    {
        if (!safeStringInput(input, sizeof(input), "\nEnter the phone number: "))
        {
            printf("✖ Input error! Please try again.\n");
            sleep(2);
            continue;
        }

        if (!validatePhone(input))
        {
            sleep(2);
            continue;
        }

        strcpy(r.phone, input); // store phone as string
        break;
    } while (1);

    // Validate amount input
    do
    {
        if (!safeStringInput(input, sizeof(input), "\nEnter amount to deposit: $"))
        {
            printf("✖ Input error! Please try again.\n");
            sleep(2);
            continue;
        }

        if (!validateAmount(input))
        {
            sleep(2);
            continue;
        }

        r.amount = atof(input);
        break;
    } while (1);

    // Validate account type input
    do
    {
        printf("\nChoose the type of account:\n");
        printf("\t-> saving\n\t-> current\n\t-> fixed01\t(for 1 year)\n\t-> fixed02\t(for 2 years)\n\t-> fixed03 \t(for 3 years)\n");

        if (!safeStringInput(r.accountType, sizeof(r.accountType), "\nEnter your choice: "))
        {
            printf("✖ Input error! Please try again.\n");
            sleep(2);
            continue;
        }

        if (!validateAccountType(r.accountType))
            sleep(2);
    } while (!validateAccountType(r.accountType));

    // Insert new account record
    char insert_sql[] = "INSERT INTO records (user_id, user_name, account_id, deposit_date, country, phone, balance, account_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL);

    if (rc == SQLITE_OK)
    {
        char date_str[20];
        sprintf(date_str, "%02d/%02d/%04d", r.deposit.month, r.deposit.day, r.deposit.year);

        sqlite3_bind_int(stmt, 1, u.id);
        sqlite3_bind_text(stmt, 2, u.name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, r.accountNbr);
        sqlite3_bind_text(stmt, 4, date_str, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, r.country, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, r.phone, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 7, r.amount);
        sqlite3_bind_text(stmt, 8, r.accountType, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc == SQLITE_DONE)
        {
            success(u);
        }
        else
        {
            printf("✖ Error creating account: %s\n", sqlite3_errmsg(db));
            sleep(3);
            mainMenu(u);
        }
    }
    else
    {
        printf("✖ Database error: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
    }
}

void checkAllAccounts(struct User u)
{
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;

    char sql[] = "SELECT account_id, deposit_date, country, phone, balance, account_type FROM records WHERE user_id = ?";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("Database error: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
        return;
    }

    sqlite3_bind_int(stmt, 1, u.id);

    system("clear");
    printf("\t\t====== All accounts from user, %s =====\n\n", u.name);

    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        found = 1;
        printf("_____________________\n");
        printf("\nAccount number: %d\n", sqlite3_column_int(stmt, 0));
        printf("Deposit Date    : %s\n", sqlite3_column_text(stmt, 1));
        printf("Country         : %s\n", sqlite3_column_text(stmt, 2));
        printf("Phone number    : %s\n", sqlite3_column_text(stmt, 3));
        printf("Amount deposited: $%.2f\n", sqlite3_column_double(stmt, 4));
        printf("Type Of Account : %s\n", sqlite3_column_text(stmt, 5));
    }

    sqlite3_finalize(stmt);

    if (!found)
    {
        printf("No accounts found for user %s\n", u.name);
    }

    success(u);
}

void updateAccountInfo(struct User u)
{
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;
    int accountId;
    int updateChoice;
    char newValue[100];

    system("clear");
    printf("\t\t\t===== Update Account Information =====\n");

    // First, show user's accounts
    char list_sql[] = "SELECT account_id, country, phone FROM records WHERE user_id = ?";
    int rc = sqlite3_prepare_v2(db, list_sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        printf("Database error: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
        return;
    }

    sqlite3_bind_int(stmt, 1, u.id);

    printf("\nYour accounts:\n");
    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        found = 1;
        printf("Account ID: %d, Country: %s, Phone: %s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2));
    }
    sqlite3_finalize(stmt);

    if (!found)
    {
        printf("No accounts found!\n");
        sleep(2);
        mainMenu(u);
        return;
    }

    printf("\nEnter the account ID you want to update: ");
    scanf("%d", &accountId);

    // Verify account belongs to user
    char verify_sql[] = "SELECT COUNT(*) FROM records WHERE account_id = ? AND user_id = ?";
    rc = sqlite3_prepare_v2(db, verify_sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_int(stmt, 2, u.id);

    rc = sqlite3_step(stmt);
    int accountExists = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (accountExists == 0)
    {
        printf("✖ Account not found or doesn't belong to you!\n");
        sleep(2);
        stayOrReturn(0, updateAccountInfo, u);
        return;
    }

    printf("\nWhat would you like to update?\n");
    printf("1. Country\n");
    printf("2. Phone number\n");
    printf("Enter your choice (1 or 2): ");
    scanf("%d", &updateChoice);

    if (updateChoice == 1)
    {
        do
        {
            printf("Enter new country: ");
            scanf("%s", newValue);
            if (!validateName(newValue))
            {
                sleep(2);
                continue;
            }
            break;
        } while (1);

        char update_sql[] = "UPDATE records SET country = ? WHERE account_id = ? AND user_id = ?";
        rc = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, newValue, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, accountId);
        sqlite3_bind_int(stmt, 3, u.id);
    }
    else if (updateChoice == 2)
    {
        do
        {
            printf("Enter new phone number: ");
            scanf("%s", newValue);
            if (!validatePhone(newValue))
            {
                sleep(2);
                continue;
            }
            break;
        } while (1);

        char update_sql[] = "UPDATE records SET phone = ? WHERE account_id = ? AND user_id = ?";
        rc = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, newValue, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, accountId);
        sqlite3_bind_int(stmt, 3, u.id);
    }
    else
    {
        printf("✖ Invalid choice!\n");
        sleep(2);
        updateAccountInfo(u);
        return;
    }

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        printf("✔ Account information updated successfully!\n");
        success(u);
    }
    else
    {
        printf("✖ Error updating account: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
    }
}