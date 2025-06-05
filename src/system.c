#include "header.h"

void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
    int option;
    if (notGood == 0)
    {
        clearScreen();
        printf("\n✖ Record not found!!\n");
    invalid:
        if (!safeIntInput(&option, "\nEnter 0 to try again, 1 to return to main menu and 2 to exit: "))
        {
            showRetryMessage();
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
            showValidationError("operation", "Please choose 1, 2, or 3.");
            goto invalid;
        }
    }
    else
    {
        if (!safeIntInput(&option, "\nEnter 1 to go to the main menu and 0 to exit: "))
        {
            showRetryMessage();
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
        showThankYouMessage();
    }
}

void success(struct User u)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    if (!safeIntInput(&option, "Enter 1 to go to the main menu and 0 to exit: "))
    {
        showRetryMessage();
        goto invalid;
    }

    system("clear");
    if (option == 1)
    {
        mainMenu(u);
    }
    else if (option == 0)
    {
        showThankYouMessage();
    }
    else
    {
        showValidationError("operation", "Please choose 1 or 0.");
        goto invalid;
    }
}

void createNewAcc(struct User u)
{
    struct Record r;
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;
    char input[100];

    clearScreen();
    showAccountCreationHeader();

    // Validate date input
    int isValidDate = 0;
    do
    {
        int month, day, year;
        if (!safeIntInput(&month, "\nEnter today's month (mm): "))
        {
            showRetryMessage();
            continue;
        }

        if (!safeIntInput(&day, "Enter today's day (dd): "))
        {
            showRetryMessage();
            continue;
        }

        if (!safeIntInput(&year, "Enter today's year (yyyy): "))
        {
            showRetryMessage();
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
            showInputErrorMessage();
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
            showValidationError("account number", "This Account is already in use. Please choose a different one.");
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
            showInputErrorMessage();
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
            showInputErrorMessage();
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
            showInputErrorMessage();
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
    int accountTypeChoice;
    do
    {
        showAccountTypeMenu();
        if (!safeIntInput(&accountTypeChoice, "\nEnter your choice (1-5): "))
        {
            showRetryMessage();
            continue;
        }

        // Map the choice to account type string
        switch (accountTypeChoice)
        {
        case 1:
            strcpy(r.accountType, "saving");
            break;
        case 2:
            strcpy(r.accountType, "current");
            break;
        case 3:
            strcpy(r.accountType, "fixed01");
            break;
        case 4:
            strcpy(r.accountType, "fixed02");
            break;
        case 5:
            strcpy(r.accountType, "fixed03");
            break;
        default:
            showValidationError("choice", "Please select a number between 1 and 5.");
            continue;
        }

        // Show confirmation of selected account type
        printf("\n✔ Selected Account Type: %s\n", r.accountType);

        // Show additional info for fixed accounts
        showAccountTypeInfo(accountTypeChoice);

        break;
    } while (1);

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
    printf("\t\t\t===== Account Portfolio Overview =====\n");
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Account Owner: %s\n", u.name);
    printf("═══════════════════════════════════════════════════════════════\n");

    int found = 0;
    int accountNumber = 1;
    double totalBalance = 0.0;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        found = 1;
        double balance = sqlite3_column_double(stmt, 4);
        totalBalance += balance;

        printf("\n[%d] ══════════════════════════════════════════════════════════\n", accountNumber);
        printf("Account Number  : %d\n", sqlite3_column_int(stmt, 0));
        printf("Account Type    : %s\n", sqlite3_column_text(stmt, 5));
        printf("Deposit Date    : %s\n", sqlite3_column_text(stmt, 1));
        printf("Country         : %s\n", sqlite3_column_text(stmt, 2));
        printf("Phone Number    : %s\n", sqlite3_column_text(stmt, 3));
        printf("Current Balance : $%.2f\n", balance);

        // Display account status based on type
        const char *accountType = (const char *)sqlite3_column_text(stmt, 5);
        if (strstr(accountType, "fixed") != NULL)
        {
            printf("Status          : Fixed Term (No transactions allowed)\n");
        }
        else if (strcmp(accountType, "saving") == 0)
        {
            printf("Status          : Active Savings Account\n");
        }
        else if (strcmp(accountType, "current") == 0)
        {
            printf("Status          : Active Current Account\n");
        }
        else
        {
            printf("Status          : Active\n");
        }

        printf("──────────────────────────────────────────────────────────────\n");
        accountNumber++;
    }

    sqlite3_finalize(stmt);

    if (!found)
    {
        printf("\n📋 No accounts found for user: %s\n", u.name);
        printf("\n💡 TIP: Create your first account using option [1] from the main menu.\n");
        printf("═══════════════════════════════════════════════════════════════\n");
    }
    else
    {
        printf("\n═══════════════════ Portfolio Summary ═══════════════════════\n");
        printf("Total Accounts  : %d\n", accountNumber - 1);
        printf("Combined Balance: $%.2f\n", totalBalance);
        printf("═══════════════════════════════════════════════════════════════\n");

        if (accountNumber > 2) // More than 1 account
        {
            printf("\n💼 Portfolio Diversification:\n");
            printf("   • You have multiple accounts for better financial management\n");
            printf("   • Consider different account types for various savings goals\n");
        }

        printf("\n📊 Quick Actions Available:\n");
        printf("   • View detailed account info: Use option [3]\n");
        printf("   • Make transactions: Use option [5]\n");
        printf("   • Update account details: Use option [2]\n");
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
    int accountIds[100];      // Array to store account IDs
    char countries[100][100]; // Array to store countries
    char phones[100][100];    // Array to store phone numbers
    int accountCount = 0;

    clearScreen();
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
        accountIds[accountCount] = sqlite3_column_int(stmt, 0); // Store account ID

        const char *country = (const char *)sqlite3_column_text(stmt, 1);
        const char *phone = (const char *)sqlite3_column_text(stmt, 2);

        // Copy the data to our arrays
        strncpy(countries[accountCount], country ? country : "", sizeof(countries[accountCount]) - 1);
        countries[accountCount][sizeof(countries[accountCount]) - 1] = '\0';

        strncpy(phones[accountCount], phone ? phone : "", sizeof(phones[accountCount]) - 1);
        phones[accountCount][sizeof(phones[accountCount]) - 1] = '\0';

        accountCount++;

        printf("[%d] Account ID: %d\n", accountCount, accountIds[accountCount - 1]);
        showUpdateDetails(countries[accountCount - 1], phones[accountCount - 1]);
    }
    sqlite3_finalize(stmt);

    if (!found)
    {
        showErrorMessage("No accounts found for your user ID.");
        mainMenu(u);
        return;
    }

    // Get user's account selection
    do
    {
        if (!safeIntInput(&accountChoice, "\nSelect account to update (enter the number): "))
        {
            showRetryMessage();
            continue;
        }

        if (accountChoice < 1 || accountChoice > accountCount)
        {
            showInvalidSelectionError(accountCount);
            continue;
        }
        break;
    } while (1);

    // Display selection options
    int selectedAccountId = accountIds[accountChoice - 1];
    const char *country = countries[accountChoice - 1];
    const char *phone = phones[accountChoice - 1];

    clearScreen();
    showUpdateMenu(selectedAccountId, country, phone);
    do
    {
        if (!safeIntInput(&updateChoice, "\nEnter your choice (1 or 2): "))
        {
            showRetryMessage();
            continue;
        }

        if (updateChoice != 1 && updateChoice != 2)
        {
            showValidationError("choice", "Please select 1 or 2.");
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
                showInputErrorMessage();
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
        printf("\n→ Updating country from %s to: %s\n", country, newValue);
    }
    else if (updateChoice == 2)
    {
        printf("\n--- Updating Phone Number ---\n");
        do
        {
            if (!safeStringInput(newValue, sizeof(newValue), "Enter new phone number: "))
            {
                showInputErrorMessage();
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
        printf("\n→ Updating phone number from %s to: %s\n", phone, newValue);
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

    clearScreen();
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
    showAccountSelectionHeader();

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
        showErrorMessage("No accounts found for your user ID.");
        mainMenu(u);
        return;
    }

    // Get user's account selection
    do
    {
        if (!safeIntInput(&accountChoice, "\nSelect account to check details (enter the number): "))
        {
            showRetryMessage();
            continue;
        }

        if (accountChoice < 1 || accountChoice > accountCount)
        {
            showInvalidSelectionError(accountCount);
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
        // Retrieve account details
        int accountId = sqlite3_column_int(stmt, 0);
        const char *depositDate = (const char *)sqlite3_column_text(stmt, 1);
        const char *country = (const char *)sqlite3_column_text(stmt, 2);
        const char *phone = (const char *)sqlite3_column_text(stmt, 3);
        double balance = sqlite3_column_double(stmt, 4);
        const char *accountType = (const char *)sqlite3_column_text(stmt, 5);

        clearScreen();
        showAccountDetails(accountId, depositDate, country, phone, balance, accountType);
        displayInterestInfo(accountType, balance, depositDate); // Display interest information
        sqlite3_finalize(stmt);
        success(u);
    }
    else
    {
        sqlite3_finalize(stmt);
        showErrorMessage("Error retrieving account details.");
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

    showAccountSelectionHeader();
    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && accountCount < 100)
    {
        found = 1;
        accountCount++;
        accountIds[accountCount - 1] = sqlite3_column_int(stmt, 0); // Store account ID
        showAccountItemWithDetails(accountCount, accountIds[accountCount - 1],
                                   (const char *)sqlite3_column_text(stmt, 5),  // account type
                                   sqlite3_column_double(stmt, 4),              // account balance
                                   (const char *)sqlite3_column_text(stmt, 2),  // country
                                   (const char *)sqlite3_column_text(stmt, 3)); // phone number
    }
    sqlite3_finalize(stmt);

    if (!found)
    {
        showErrorMessage("No accounts available for transactions!");
        mainMenu(u);
        return;
    }

    // Get user's account selection
    do
    {
        if (!safeIntInput(&accountChoice, "\nSelect account for transaction (enter the number): "))
        {
            showRetryMessage();
            continue;
        }

        if (accountChoice < 1 || accountChoice > accountCount)
        {
            showInvalidSelectionError(accountCount);
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
        showErrorMessage("Transactions are not allowed on fixed accounts (fixed01, fixed02, fixed03)!");
        mainMenu(u);
        return;
    }

    // Display selected account info
    clearScreen();
    showTransactionMenu(selectedAccountId, accountType, currentBalance);
    do
    {
        if (!safeIntInput(&transactionType, "\nEnter your choice (1 or 2): "))
        {
            showRetryMessage();
            continue;
        }

        if (transactionType != 1 && transactionType != 2)
        {
            showValidationError("transaction type", "Please select 1 or 2.");
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
            showInputErrorMessage();
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
    const char *transactionTypeStr = (transactionType == 1) ? "Deposit" : "Withdrawal";
    showTransactionSummary(selectedAccountId, transactionTypeStr, amount, currentBalance, newBalance);
    int confirm;
    do
    {
        if (!safeIntInput(&confirm, "\nConfirm transaction? [1] Yes [2] No: "))
        {
            showRetryMessage();
            continue;
        }

        if (confirm == 2)
        {
            showErrorMessage("Transaction cancelled by user.");
            mainMenu(u);
            return;
        }
        else if (confirm != 1)
        {
            showValidationError("choice", "Please enter 1 for Yes or 2 for No.");
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
        const char *transactionTypeStr = (transactionType == 1) ? "Deposited" : "Withdrew";
        showTransactionResult(selectedAccountId, transactionTypeStr, amount, newBalance);
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

    clearScreen();
    showRemovalHeader();

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

    showAccountSelectionHeader();
    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && accountCount < 100)
    {
        found = 1;
        accountCount++;
        accountIds[accountCount - 1] = sqlite3_column_int(stmt, 0); // Store account ID
        showAccountItemWithDetails(accountCount, accountIds[accountCount - 1],
                                   (const char *)sqlite3_column_text(stmt, 5),
                                   sqlite3_column_double(stmt, 4),
                                   (const char *)sqlite3_column_text(stmt, 2),
                                   (const char *)sqlite3_column_text(stmt, 3));
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
            showRetryMessage();
            continue;
        }

        if (accountChoice < 1 || accountChoice > accountCount)
        {
            showInvalidSelectionError(accountCount);
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

    // Store account details for display and later use
    int accountId = sqlite3_column_int(stmt, 0);
    const char *depositDate = (const char *)sqlite3_column_text(stmt, 1);
    const char *country = (const char *)sqlite3_column_text(stmt, 2);
    const char *phone = (const char *)sqlite3_column_text(stmt, 3);
    double balance = sqlite3_column_double(stmt, 4);
    const char *accountType = (const char *)sqlite3_column_text(stmt, 5);

    // Display account details for confirmation
    clearScreen();
    showRemovalWarning(accountId, depositDate, country, phone, balance, accountType);
    showRemovalConfirmationMenu();

    sqlite3_finalize(stmt);
    int deleteChoice;
    do
    {
        if (!safeIntInput(&deleteChoice, "\nEnter your choice (1 or 2): "))
        {
            showRetryMessage();
            continue;
        }

        if (deleteChoice == 2)
        {
            showCancelledAction("deletion", u);
        }
        else if (deleteChoice != 1)
        {
            showValidationError("choice", "Please enter 1 to proceed or 2 to cancel.");
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
            showInputErrorMessage();
            continue;
        }

        if (strcmp(confirmation, "DELETE") != 0)
        {
            showConfirmationInput("DELETE");
            continue;
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
        clearScreen();
        showRemovalSuccess(selectedAccountId, balance);
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
    int accountChoice;
    char targetUsername[50];
    int targetUserId;
    int accountIds[100]; // Array to store account IDs
    int accountCount = 0;

    clearScreen();
    showTransferHeader();

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

    showAccountSelectionHeader();
    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && accountCount < 100)
    {
        found = 1;
        accountCount++;
        accountIds[accountCount - 1] = sqlite3_column_int(stmt, 0); // Store account ID
        showAccountItemWithDetails(accountCount, accountIds[accountCount - 1],
                                   (const char *)sqlite3_column_text(stmt, 5),  // account type
                                   sqlite3_column_double(stmt, 4),              // account balance
                                   (const char *)sqlite3_column_text(stmt, 2),  // country
                                   (const char *)sqlite3_column_text(stmt, 3)); // phone number
    }
    sqlite3_finalize(stmt);

    if (!found)
    {
        printf("No accounts found for transfer!\n");
        sleep(2);
        mainMenu(u);
        return;
    }

    // Get user's account selection
    do
    {
        if (!safeIntInput(&accountChoice, "\nSelect account to transfer (enter the number): "))
        {
            showRetryMessage();
            continue;
        }

        if (accountChoice < 1 || accountChoice > accountCount)
        {
            showInvalidSelectionError(accountCount);
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
        stayOrReturn(0, transferOwnership, u);
        return;
    }

    // Store account details for later display
    int accountId = sqlite3_column_int(stmt, 0);
    char depositDate[20], country[50], phone[20], accountType[20];
    strcpy(depositDate, (char *)sqlite3_column_text(stmt, 1));
    strcpy(country, (char *)sqlite3_column_text(stmt, 2));
    strcpy(phone, (char *)sqlite3_column_text(stmt, 3));
    strcpy(accountType, (char *)sqlite3_column_text(stmt, 5));
    double balance = sqlite3_column_double(stmt, 4);
    sqlite3_finalize(stmt);

    clearScreen();
    showTransferAccountDisplay(selectedAccountId, accountType, balance);

    do
    {
        if (!safeStringInput(targetUsername, sizeof(targetUsername), "\nEnter recipient's username: "))
        {
            showInputErrorMessage();
            continue;
        }

        // Check if user is trying to transfer to themselves
        if (strcmp(targetUsername, u.name) == 0)
        {
            printf("✖ Cannot transfer account to yourself!\n");
            sleep(2);
            continue;
        }

        // Find target user ID
        char user_sql[] = "SELECT id FROM users WHERE name = ?";
        rc = sqlite3_prepare_v2(db, user_sql, -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, targetUsername, -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);

        if (rc != SQLITE_ROW)
        {
            sqlite3_finalize(stmt);
            printf("✖ User '%s' not found in the system!\n", targetUsername);
            printf("Please verify the username and try again.\n");
            sleep(2);
            continue;
        }

        targetUserId = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        break;
    } while (1);

    // Show transfer confirmation
    clearScreen();
    showTransferConfirmation(accountId, accountType, balance, country, phone, depositDate,
                             u.name, targetUsername);

    showTransferConfirmationMenu();
    int transferChoice;
    do
    {
        if (!safeIntInput(&transferChoice, "\nEnter your choice (1 or 2): "))
        {
            showRetryMessage();
            continue;
        }

        if (transferChoice == 2)
        {
            showCancelledAction("transfer", u);
        }
        else if (transferChoice != 1)
        {
            showValidationError("choice", "Please enter 1 to proceed or 2 to cancel.");
            continue;
        }
        break;
    } while (1);

    // Final confirmation with text input
    printf("\n--- Final Security Confirmation ---\n");
    char confirmation[20];
    do
    {
        if (!safeStringInput(confirmation, sizeof(confirmation), "Type 'TRANSFER' to confirm ownership transfer: "))
        {
            showInputErrorMessage();
            continue;
        }

        if (strcmp(confirmation, "TRANSFER") != 0)
        {
            showConfirmationInput("TRANSFER");
            continue;
        }
        break;
    } while (1);

    // Proceed with transfer
    printf("\n→ Processing ownership transfer...\n");

    char transfer_sql[] = "UPDATE records SET user_id = ?, user_name = ? WHERE account_id = ? AND user_id = ?";
    rc = sqlite3_prepare_v2(db, transfer_sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        printf("✖ Database error: %s\n", sqlite3_errmsg(db));
        sleep(2);
        mainMenu(u);
        return;
    }

    sqlite3_bind_int(stmt, 1, targetUserId);
    sqlite3_bind_text(stmt, 2, targetUsername, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, selectedAccountId);
    sqlite3_bind_int(stmt, 4, u.id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        clearScreen();
        showTransferSuccess(selectedAccountId, accountType, balance, u.name, targetUsername);
        success(u);
    }
    else
    {
        printf("✖ Error transferring account ownership: %s\n", sqlite3_errmsg(db));
        sleep(3);
        stayOrReturn(0, transferOwnership, u);
    }
}