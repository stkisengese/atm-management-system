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
    int accountChoice;
    int updateChoice;
    char newValue[100];
    int accountIds[100]; // Array to store account IDs
    int accountCount = 0;

    system("clear");
    printf("\t\t\t===== Update Account Information =====\n");

    // First, show user's accounts with selection numbers
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
    printf("═══════════════════════════════════════════════════════════════\n");
    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && accountCount < 100)
    {
        found = 1;
        accountCount++;
        accountIds[accountCount - 1] = sqlite3_column_int(stmt, 0); // Store account ID

        printf("[%d] Account ID: %d\n", accountCount, accountIds[accountCount - 1]);
        printf("    Country: %s\n", sqlite3_column_text(stmt, 1));
        printf("    Phone: %s\n", sqlite3_column_text(stmt, 2));
        printf("───────────────────────────────────────────────────────────────\n");
    }
    sqlite3_finalize(stmt);

    if (!found)
    {
        printf("No accounts found!\n");
        sleep(2);
        mainMenu(u);
        return;
    }

    // Get user's account selection
    do
    {
        if (!safeIntInput(&accountChoice, "\nSelect account to update (enter the number): "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        if (accountChoice < 1 || accountChoice > accountCount)
        {
            printf("✖ Invalid selection! Please choose a number between 1 and %d.\n", accountCount);
            sleep(2);
            continue;
        }
        break;
    } while (1);

    // Display selection options
    int selectedAccountId = accountIds[accountChoice - 1];
    printf("\n✔ Selected Account ID: %d\n", selectedAccountId);
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("What would you like to update?\n");
    printf("[1] Country\n");
    printf("[2] Phone number\n");
    printf("═══════════════════════════════════════════════════════════════\n");

    do
    {
        if (!safeIntInput(&updateChoice, "\nEnter your choice (1 or 2): "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        if (updateChoice != 1 && updateChoice != 2)
        {
            printf("✖ Invalid choice! Please select 1 or 2.\n");
            sleep(2);
            continue;
        }
        break;
    } while (1);

    if (updateChoice == 1)
    {
        printf("\n--- Updating Country ---\n");
        do
        {
            if (!safeStringInput(newValue, sizeof(newValue), "Enter new country: "))
            {
                printf("✖ Input error! Please try again.\n");
                sleep(2);
                continue;
            }

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
        sqlite3_bind_int(stmt, 2, selectedAccountId);
        sqlite3_bind_int(stmt, 3, u.id);

        printf("\n→ Updating country from previous value to: %s\n", newValue);
    }
    else if (updateChoice == 2)
    {
        printf("\n--- Updating Phone Number ---\n");
        do
        {
            if (!safeStringInput(newValue, sizeof(newValue), "Enter new phone number: "))
            {
                printf("✖ Input error! Please try again.\n");
                sleep(2);
                continue;
            }

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
        sqlite3_bind_int(stmt, 2, selectedAccountId);
        sqlite3_bind_int(stmt, 3, u.id);
        printf("\n→ Updating phone number from previous value to: %s\n", newValue);
    }

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        success(u);
    }
    else
    {
        printf("✖ Error updating account: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
    }
}

void checkAccountDetails(struct User u)
{
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;
    int accountChoice;
    int accountIds[100]; // Array to store account IDs
    int accountCount = 0;

    system("clear");
    printf("\t\t\t===== Check Account Details =====\n");

    // First, show user's accounts with selection numbers
    char list_sql[] = "SELECT account_id, deposit_date, country, phone, balance, account_type FROM records WHERE user_id = ?";
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
    printf("═══════════════════════════════════════════════════════════════\n");

    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && accountCount < 100)
    {
        found = 1;
        accountCount++;
        accountIds[accountCount - 1] = sqlite3_column_int(stmt, 0); // Store account ID

        printf("[%d] Account ID: %d (%s)\n", accountCount, accountIds[accountCount - 1],
               sqlite3_column_text(stmt, 5)); // Show account type
        printf("    Balance: $%.2f\n", sqlite3_column_double(stmt, 4));
        printf("    Country: %s | Phone: %s\n",
               sqlite3_column_text(stmt, 2), sqlite3_column_text(stmt, 3));
        printf("───────────────────────────────────────────────────────────────\n");
    }
    sqlite3_finalize(stmt);

    if (!found)
    {
        printf("No accounts found!\n");
        sleep(2);
        mainMenu(u);
        return;
    }

    // Get user's account selection
    do
    {
        if (!safeIntInput(&accountChoice, "\nSelect account to check details (enter the number): "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        if (accountChoice < 1 || accountChoice > accountCount)
        {
            printf("✖ Invalid selection! Please choose a number between 1 and %d.\n", accountCount);
            sleep(2);
            continue;
        }
        break;
    } while (1);

    int selectedAccountId = accountIds[accountChoice - 1];

    // Get detailed account information
    char detail_sql[] = "SELECT account_id, deposit_date, country, phone, balance, account_type FROM records WHERE account_id = ? AND user_id = ?";
    rc = sqlite3_prepare_v2(db, detail_sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        printf("Database error: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
        return;
    }

    sqlite3_bind_int(stmt, 1, selectedAccountId);
    sqlite3_bind_int(stmt, 2, u.id);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        system("clear");
        printf("\t\t\t===== Check Account Details =====\n");
        printf("\n════════════════════ Account Details ═════════════════════\n");
        printf("Account Number : %d\n", sqlite3_column_int(stmt, 0));
        printf("Deposit Date   : %s\n", sqlite3_column_text(stmt, 1));
        printf("Country        : %s\n", sqlite3_column_text(stmt, 2));
        printf("Phone Number   : %s\n", sqlite3_column_text(stmt, 3));
        printf("Balance        : $%.2f\n", sqlite3_column_double(stmt, 4));
        printf("Account Type   : %s\n", sqlite3_column_text(stmt, 5));
        printf("═══════════════════════════════════════════════════════════════\n");

        const char *accountType = (const char *)sqlite3_column_text(stmt, 5);
        double balance = sqlite3_column_double(stmt, 4);
        const char *depositDate = (const char *)sqlite3_column_text(stmt, 1);
        displayInterestInfo(accountType, balance, depositDate);
        sqlite3_finalize(stmt);
        success(u);
    }
    else
    {
        sqlite3_finalize(stmt);
        printf("✖ Error retrieving account details.\n");
        sleep(2);
        mainMenu(u);
    }
}

void makeTransaction(struct User u)
{
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;
    int accountChoice;
    int transactionType;
    char amountStr[50];
    double amount;
    double currentBalance;
    char accountType[20];
    int accountIds[100]; // Array to store account IDs
    int accountCount = 0;

    system("clear");
    printf("\t\t\t===== Make Transaction =====\n");

    // First, show user's accounts with selection numbers
    char list_sql[] = "SELECT account_id, balance, account_type FROM records WHERE user_id = ?";
    int rc = sqlite3_prepare_v2(db, list_sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        printf("Database error: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
        return;
    }

    sqlite3_bind_int(stmt, 1, u.id);

    printf("\nAvailable accounts for transactions:\n");
    printf("═══════════════════════════════════════════════════════════════\n");

    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && accountCount < 100)
    {
        found = 1;
        accountCount++;
        accountIds[accountCount - 1] = sqlite3_column_int(stmt, 0); // Store account ID

        printf("[%d] Account ID: %d (%s)\n", accountCount, accountIds[accountCount - 1],
               sqlite3_column_text(stmt, 2)); // Show account type
        printf("    Current Balance: $%.2f\n", sqlite3_column_double(stmt, 1));
        printf("───────────────────────────────────────────────────────────────\n");
    }
    sqlite3_finalize(stmt);

    if (!found)
    {
        printf("No accounts available for transactions!\n");
        printf("Note: Fixed accounts (fixed01, fixed02, fixed03) do not allow transactions.\n");
        sleep(3);
        mainMenu(u);
        return;
    }

    // Get user's account selection
    do
    {
        if (!safeIntInput(&accountChoice, "\nSelect account for transaction (enter the number): "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        if (accountChoice < 1 || accountChoice > accountCount)
        {
            printf("✖ Invalid selection! Please choose a number between 1 and %d.\n", accountCount);
            sleep(2);
            continue;
        }
        break;
    } while (1);

    int selectedAccountId = accountIds[accountChoice - 1];

    // Get selected account details
    char get_sql[] = "SELECT balance, account_type FROM records WHERE account_id = ? AND user_id = ?";
    rc = sqlite3_prepare_v2(db, get_sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, selectedAccountId);
    sqlite3_bind_int(stmt, 2, u.id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        printf("✖ Account not found!\n");
        sleep(2);
        mainMenu(u);
        return;
    }

    currentBalance = sqlite3_column_double(stmt, 0);
    strcpy(accountType, (char *)sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);

    // Check if account is fixed type
    if (isFixedAccount(accountType))
    {
        printf("✖ Transactions are not allowed on fixed accounts (fixed01, fixed02, fixed03)!\n");
        sleep(3);
        mainMenu(u);
        return;
    }

    // Display selected account info
    system("clear");
    printf("\t\t\t===== Make Transaction =====\n");
    printf("\n✔ Selected Account ID: %d (%s)\n", selectedAccountId, accountType);
    printf("Current Balance: $%.2f\n", currentBalance);

    // Transaction type selection
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Transaction Options:\n");
    printf("[1] Deposit Money\n");
    printf("[2] Withdraw Money\n");
    printf("═══════════════════════════════════════════════════════════════\n");

    do
    {
        if (!safeIntInput(&transactionType, "\nEnter your choice (1 or 2): "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        if (transactionType != 1 && transactionType != 2)
        {
            printf("✖ Invalid transaction type! Please choose 1 or 2.\n");
            sleep(2);
            continue;
        }
        break;
    } while (1);

    // Amount input with validation
    printf("\n--- %s Transaction ---\n", (transactionType == 1) ? "Deposit" : "Withdrawal");

    do
    {
        if (!safeStringInput(amountStr, sizeof(amountStr), "Enter amount: $"))
        {
            printf("✖ Input error! Please try again.\n");
            sleep(2);
            continue;
        }

        if (!validateAmount(amountStr))
        {
            sleep(2);
            continue;
        }

        amount = atof(amountStr);

        // Additional validation for withdrawal
        if (transactionType == 2 && amount > currentBalance)
        {
            printf("✖ Insufficient funds! Current balance: $%.2f\n", currentBalance);
            printf("Maximum withdrawal amount: $%.2f\n", currentBalance);
            sleep(2);
            continue;
        }

        break;
    } while (1);

    // Calculate new balance
    double newBalance;
    if (transactionType == 1) // Deposit
    {
        newBalance = currentBalance + amount;
        printf("\n→ Processing deposit of $%.2f...\n", amount);
    }
    else // Withdraw
    {
        newBalance = currentBalance - amount;
        printf("\n→ Processing withdrawal of $%.2f...\n", amount);
    }

    // Show transaction summary before confirmation
    printf("\n═══════════════ Transaction Summary ═══════════════\n");
    printf("Account ID      : %d\n", selectedAccountId);
    printf("Transaction Type: %s\n", (transactionType == 1) ? "Deposit" : "Withdrawal");
    printf("Amount          : $%.2f\n", amount);
    printf("Current Balance : $%.2f\n", currentBalance);
    printf("New Balance     : $%.2f\n", newBalance);
    printf("═══════════════════════════════════════════════════\n");

    int confirm;
    do
    {
        if (!safeIntInput(&confirm, "\nConfirm transaction? [1] Yes [2] No: "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        if (confirm == 2)
        {
            printf("Transaction cancelled.\n");
            sleep(2);
            mainMenu(u);
            return;
        }
        else if (confirm != 1)
        {
            printf("✖ Invalid choice! Please enter 1 for Yes or 2 for No.\n");
            sleep(2);
            continue;
        }
        break;
    } while (1);

    // Update balance in database
    char update_sql[] = "UPDATE records SET balance = ? WHERE account_id = ? AND user_id = ?";
    rc = sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    sqlite3_bind_double(stmt, 1, newBalance);
    sqlite3_bind_int(stmt, 2, selectedAccountId);
    sqlite3_bind_int(stmt, 3, u.id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        printf("\n✔ Transaction completed successfully!\n");
        printf("Account ID: %d\n", selectedAccountId);
        printf("Transaction: %s $%.2f\n", (transactionType == 1) ? "Deposited" : "Withdrew", amount);
        printf("New Balance: $%.2f\n", newBalance);
        success(u);
    }
    else
    {
        printf("✖ Transaction failed: %s\n", sqlite3_errmsg(db));
        sleep(3);
        mainMenu(u);
    }
}

void removeAccount(struct User u)
{
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;
    int accountChoice;
    char confirmation[10];
    int accountIds[100]; // Array to store account IDs
    int accountCount = 0;

    system("clear");
    printf("\t\t\t===== Remove Account =====\n");

    // First, show user's accounts with selection numbers
    char list_sql[] = "SELECT account_id, deposit_date, country, phone, balance, account_type FROM records WHERE user_id = ?";
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
    printf("═══════════════════════════════════════════════════════════════\n");

    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && accountCount < 100)
    {
        found = 1;
        accountCount++;
        accountIds[accountCount - 1] = sqlite3_column_int(stmt, 0); // Store account ID

        printf("[%d] Account ID: %d (%s)\n", accountCount, accountIds[accountCount - 1],
               sqlite3_column_text(stmt, 5)); // Show account type
        printf("    Balance: $%.2f\n", sqlite3_column_double(stmt, 4));
        printf("    Country: %s | Phone: %s\n",
               sqlite3_column_text(stmt, 2), sqlite3_column_text(stmt, 3));
        printf("───────────────────────────────────────────────────────────────\n");
    }
    sqlite3_finalize(stmt);

    if (!found)
    {
        printf("No accounts found!\n");
        sleep(2);
        mainMenu(u);
        return;
    }

    // Get user's account selection
    do
    {
        if (!safeIntInput(&accountChoice, "\nSelect account to remove (enter the number): "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        if (accountChoice < 1 || accountChoice > accountCount)
        {
            printf("✖ Invalid selection! Please choose a number between 1 and %d.\n", accountCount);
            sleep(2);
            continue;
        }
        break;
    } while (1);

    int selectedAccountId = accountIds[accountChoice - 1];

    // Get detailed account information for confirmation
    char detail_sql[] = "SELECT account_id, deposit_date, country, phone, balance, account_type FROM records WHERE account_id = ? AND user_id = ?";
    rc = sqlite3_prepare_v2(db, detail_sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        printf("Database error: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
        return;
    }

    sqlite3_bind_int(stmt, 1, selectedAccountId);
    sqlite3_bind_int(stmt, 2, u.id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        printf("✖ Account not found!\n");
        sleep(2);
        stayOrReturn(0, removeAccount, u);
        return;
    }

    // Display account details for confirmation
    system("clear");
    printf("\t\t\t===== Remove Account =====\n");
    printf("\n⚠️  WARNING: Account Deletion Confirmation\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Account Number : %d\n", sqlite3_column_int(stmt, 0));
    printf("Deposit Date   : %s\n", sqlite3_column_text(stmt, 1));
    printf("Country        : %s\n", sqlite3_column_text(stmt, 2));
    printf("Phone Number   : %s\n", sqlite3_column_text(stmt, 3));
    printf("Balance        : $%.2f\n", sqlite3_column_double(stmt, 4));
    printf("Account Type   : %s\n", sqlite3_column_text(stmt, 5));
    printf("═══════════════════════════════════════════════════════════════\n");

    double balance = sqlite3_column_double(stmt, 4);
    sqlite3_finalize(stmt);

    printf("\n🚨 CRITICAL WARNING:\n");
    printf("• This action will PERMANENTLY delete this account\n");
    printf("• Account balance of $%.2f will be LOST\n", balance);
    printf("• This action CANNOT be undone\n");
    printf("• All account history will be removed\n");

    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Deletion Confirmation:\n");
    printf("[1] Proceed with deletion\n");
    printf("[2] Cancel and return to main menu\n");
    printf("═══════════════════════════════════════════════════════════════\n");

    int deleteChoice;
    do
    {
        if (!safeIntInput(&deleteChoice, "\nEnter your choice (1 or 2): "))
        {
            printf("✖ Invalid input! Please try again.\n");
            sleep(2);
            continue;
        }

        if (deleteChoice == 2)
        {
            printf("\n✔ Account deletion cancelled. Returning to main menu...\n");
            sleep(2);
            mainMenu(u);
            return;
        }
        else if (deleteChoice != 1)
        {
            printf("✖ Invalid choice! Please enter 1 to proceed or 2 to cancel.\n");
            sleep(2);
            continue;
        }
        break;
    } while (1);

    // Final confirmation with text input
    printf("\n--- Final Security Confirmation ---\n");
    do
    {
        if (!safeStringInput(confirmation, sizeof(confirmation), "Type 'DELETE' to confirm permanent deletion: "))
        {
            printf("✖ Input error! Please try again.\n");
            sleep(2);
            continue;
        }

        if (strcmp(confirmation, "DELETE") != 0)
        {
            printf("✖ Confirmation failed! You must type 'DELETE' exactly.\n");
            printf("Account deletion cancelled. Returning to main menu...\n");
            sleep(3);
            mainMenu(u);
            return;
        }
        break;
    } while (1);

    // Proceed with deletion
    printf("\n→ Processing account deletion...\n");

    char delete_sql[] = "DELETE FROM records WHERE account_id = ? AND user_id = ?";
    rc = sqlite3_prepare_v2(db, delete_sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        printf("✖ Database error: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
        return;
    }

    sqlite3_bind_int(stmt, 1, selectedAccountId);
    sqlite3_bind_int(stmt, 2, u.id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        system("clear");
        printf("\t\t\t===== Account Deletion Complete =====\n");
        printf("\n✔ SUCCESS: Account has been permanently deleted!\n");
        printf("\n═══════════════ Deletion Summary ═══════════════\n");
        printf("Deleted Account ID: %d\n", selectedAccountId);
        printf("Previous Balance  : $%.2f\n", balance);
        printf("Status           : Permanently Removed\n");
        printf("═══════════════════════════════════════════════\n");

        success(u);
    }
    else
    {
        printf("✖ Error deleting account: %s\n", sqlite3_errmsg(db));
        sleep(3);
        stayOrReturn(0, removeAccount, u);
    }
}

void transferOwnership(struct User u)
{
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;
    int accountId;
    char targetUsername[50];
    int targetUserId;

    system("clear");
    printf("\t\t\t===== Transfer Account Ownership =====\n");

    // Show user's accounts
    char list_sql[] = "SELECT account_id, balance, account_type FROM records WHERE user_id = ?";
    int rc = sqlite3_prepare_v2(db, list_sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, u.id);

    printf("Your accounts:\n");
    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        found = 1;
        printf("Account ID: %d, Balance: $%.2f, Type: %s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_double(stmt, 1),
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

    printf("\nEnter account ID to transfer: ");
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
        printf("✖ Account not found!\n");
        sleep(2);
        stayOrReturn(0, transferOwnership, u);
        return;
    }

    printf("Enter username to transfer to: ");
    scanf("%s", targetUsername);

    // Find target user ID
    char user_sql[] = "SELECT id FROM users WHERE name = ?";
    rc = sqlite3_prepare_v2(db, user_sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, targetUsername, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        printf("✖ Target user '%s' not found!\n", targetUsername);
        sleep(2);
        transferOwnership(u);
        return;
    }

    targetUserId = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (targetUserId == u.id)
    {
        printf("✖ Cannot transfer account to yourself!\n");
        sleep(2);
        transferOwnership(u);
        return;
    }

    // Transfer ownership
    char transfer_sql[] = "UPDATE records SET user_id = ?, user_name = ? WHERE account_id = ? AND user_id = ?";
    rc = sqlite3_prepare_v2(db, transfer_sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, targetUserId);
    sqlite3_bind_text(stmt, 2, targetUsername, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, accountId);
    sqlite3_bind_int(stmt, 4, u.id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        printf("✔ Account %d has been successfully transferred to %s!\n", accountId, targetUsername);
        success(u);
    }
    else
    {
        printf("✖ Error transferring account: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
    }
}