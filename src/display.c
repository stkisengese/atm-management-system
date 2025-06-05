#include "header.h"

// =====================================================
// GENERAL DISPLAY FUNCTIONS
// =====================================================

void clearScreen()
{
    system("clear");
}

void showATMHeader()
{
    printf("\n\t\t\t╔══════════════════════════╗\n");
    printf("\t\t\t║         ATM SYSTEM       ║\n");
    printf("\t\t\t╚══════════════════════════╝\n");
}

// void showWelcomeMessage()
// {
//     printf("\t\t\t===== Welcome to ATM System =====\n");
// }

void showSuccessMessage()
{
    printf("\n✔ Success!\n\n");
}

void showErrorMessage(const char *message)
{
    printf("\n\t\t ✖ %s\n", message);
    sleep(2);
}

void showWarningMessage(const char *message)
{
    printf("\n⚠️  %s\n", message);
}

void showInfoMessage(const char *message)
{
    printf("\n💡 %s\n", message);
}

void showThankYouMessage()
{
    printf("\n\t\t Thank you for using our ATM system!\n");
    printf("\t\t Exiting...\n\n");
    closeDatabase();
    exit(0);
}

// =====================================================
// MENU DISPLAY FUNCTIONS
// =====================================================

void showMainMenu(struct User u)
{
    printf("\n\t\t Welcome back, %s!\n", u.name);
    printf("\n\t\t-->> Feel free to choose one of the options below <<--\n\n");
    printf("\t\t╔═══════════════════════════════════════════╗\n");
    printf("\t\t║  [1] - Create a new account               ║\n");
    printf("\t\t║  [2] - Update account information         ║\n");
    printf("\t\t║  [3] - Check accounts                     ║\n");
    printf("\t\t║  [4] - Check list of owned account        ║\n");
    printf("\t\t║  [5] - Make Transaction                   ║\n");
    printf("\t\t║  [6] - Remove existing account            ║\n");
    printf("\t\t║  [7] - Transfer ownership                 ║\n");
    printf("\t\t║  [8] - Logout (Switch User)               ║\n");
    printf("\t\t║  [9] - Exit System                        ║\n");
    printf("\t\t╚═══════════════════════════════════════════╝\n");
}

void showLoginMenu()
{
    printf("\n\t\t Welcome to the ATM System\n");
    printf("\t\t Please select an option:\n\n");
    printf("\t\t╔═══════════════════════════════════════════╗\n");
    printf("\t\t║  [1] - Login to existing account          ║\n");
    printf("\t\t║  [2] - Register new user                  ║\n");
    printf("\t\t║  [3] - Exit system                        ║\n");
    printf("\t\t╚═══════════════════════════════════════════╝\n");
}

void showAccountTypeMenu()
{
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Choose the type of account:\n");
    printf("[1] Saving Account\n");
    printf("[2] Current Account\n");
    printf("[3] Fixed Deposit - 1 Year (fixed01)\n");
    printf("[4] Fixed Deposit - 2 Years (fixed02)\n");
    printf("[5] Fixed Deposit - 3 Years (fixed03)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
}

void showTransactionMenu(int selectedAccountId, const char *accountType,
                         double currentBalance)
{
    printf("\t\t\t===== Make Transaction =====\n");
    printf("\n✔ Selected Account ID: %d (%s)\n", selectedAccountId, accountType);
    printf("Current Balance: $%.2f\n", currentBalance);
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Transaction Options:\n");
    printf("[1] Deposit Money\n");
    printf("[2] Withdraw Money\n");
    printf("═══════════════════════════════════════════════════════════════\n");
}

void showUpdateMenu(int selectedAccountId, const char *country, const char *phoneNumber)
{
    printf("\t\t\t===== Update Account Information =====\n");
    printf("\n✔ Selected Account ID: %d\n", selectedAccountId);
    showUpdateDetails(country, phoneNumber);
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("What would you like to update?\n");
    printf("[1] Country\n");
    printf("[2] Phone number\n");
    printf("═══════════════════════════════════════════════════════════════\n");
}

void showUpdateDetails(const char *country, const char *phone)
{
    printf("    Country: %s\n", country);
    printf("    Phone: %s\n", phone);
    printf("───────────────────────────────────────────────────────────────\n");
}

// =====================================================
// ACCOUNT DISPLAY FUNCTIONS
// =====================================================

void showAccountCreationHeader()
{
    printf("\t\t\t===== New record =====\n");
}

void showAccountTypeInfo(int accountTypeChoice)
{
    switch (accountTypeChoice)
    {
    case 1:
        printf("📌 Note: Savings account features:\n");
        printf("   • Earn interest on deposits\n");
        printf("   • Suitable for long-term savings\n");
        printf("   • Flexible deposits and withdrawals\n");
        break;
    case 2:
        printf("📌 Note: Current account features:\n");
        printf("   • Designed for frequent transactions\n");
        printf("   • No interest earned\n");
        printf("   • Unlimited deposits and withdrawals\n");
        break;
    case 3:
    case 4:
    case 5:
    {
        int years = accountTypeChoice - 2;
        printf("📌 Note: Fixed deposit account for %d year(s)\n", years);
        printf("   • Higher interest rate\n");
        printf("   • No withdrawals/deposits allowed during term\n");
        printf("   • Funds locked until maturity\n");
    }
    break;
    }
}

void showAccountPortfolioHeader(const char *userName)
{
    printf("\t\t\t===== Account Portfolio Overview =====\n");
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Account Owner: %s\n", userName);
    printf("═══════════════════════════════════════════════════════════════\n");
}

void showAccountListItem(int itemNumber, int accountId, const char *accountType,
                         const char *depositDate, const char *country,
                         const char *phone, double balance)
{
    printf("\n[%d] ══════════════════════════════════════════════════════════\n", itemNumber);
    printf("Account Number  : %d\n", accountId);
    printf("Account Type    : %s\n", accountType);
    printf("Deposit Date    : %s\n", depositDate);
    printf("Country         : %s\n", country);
    printf("Phone Number    : %s\n", phone);
    printf("Current Balance : $%.2f\n", balance);

    // Display account status
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
}

void showPortfolioSummary(int totalAccounts, double totalBalance)
{
    printf("\n═══════════════════ Portfolio Summary ═══════════════════════\n");
    printf("Total Accounts  : %d\n", totalAccounts);
    printf("Combined Balance: $%.2f\n", totalBalance);
    printf("═══════════════════════════════════════════════════════════════\n");

    if (totalAccounts > 1)
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

void showNoAccountsMessage(const char *userName)
{
    printf("\n📋 No accounts found for user: %s\n", userName);
    printf("\n💡 TIP: Create your first account using option [1] from the main menu.\n");
    printf("═══════════════════════════════════════════════════════════════\n");
}

void showAccountDetails(int accountId, const char *depositDate, const char *country,
                        const char *phone, double balance, const char *accountType)
{
    printf("\t\t\t===== Check Account Details =====\n");
    printf("\n════════════════════ Account Details ═════════════════════\n");
    printf("Account Number : %d\n", accountId);
    printf("Deposit Date   : %s\n", depositDate);
    printf("Country        : %s\n", country);
    printf("Phone Number   : %s\n", phone);
    printf("Balance        : $%.2f\n", balance);
    printf("Account Type   : %s\n", accountType);
    printf("═══════════════════════════════════════════════════════════════\n");
}

// =====================================================
// TRANSACTION DISPLAY FUNCTIONS
// =====================================================

// void showTransactionHeader()
// {
//     printf("\t\t\t===== Make Transaction =====\n");
// }

// void showSelectedAccount(int accountId, const char* accountType, double balance)
// {
//     printf("\n✔ Selected Account ID: %d (%s)\n", accountId, accountType);
//     printf("Current Balance: $%.2f\n", balance);
// }

void showTransactionSummary(int accountId, const char *transactionType,
                            double amount, double currentBalance, double newBalance)
{
    printf("\n═══════════════ Transaction Summary ═══════════════\n");
    printf("Account ID      : %d\n", accountId);
    printf("Transaction Type: %s\n", transactionType);
    printf("Amount          : $%.2f\n", amount);
    printf("Current Balance : $%.2f\n", currentBalance);
    printf("New Balance     : $%.2f\n", newBalance);
    printf("═══════════════════════════════════════════════════\n");
}

void showTransactionResult(int accountId, const char *transactionType, double amount, double newBalance)
{
    printf("\n✔ Transaction completed successfully!\n");
    printf("Account ID: %d\n", accountId);
    printf("Transaction: %s $%.2f\n", transactionType, amount);
    printf("New Balance: $%.2f\n", newBalance);
}

// // =====================================================
// // UPDATE DISPLAY FUNCTIONS
// // =====================================================

// void showUpdateHeader()
// {
//     printf("\t\t\t===== Update Account Information =====\n");
// }

// void showUpdateProgress(const char* field, const char* newValue)
// {
//     printf("\n→ Updating %s from previous value to: %s\n", field, newValue);
// }

// =====================================================
// REMOVAL DISPLAY FUNCTIONS
// =====================================================

void showRemovalHeader()
{
    printf("\t\t\t===== Remove Account =====\n");
}

void showRemovalWarning(int accountId, const char *depositDate, const char *country,
                        const char *phone, double balance, const char *accountType)
{
    printf("\t\t\t===== Remove Account =====\n");
    printf("\n⚠️  WARNING: Account Deletion Confirmation\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Account Number : %d\n", accountId);
    printf("Deposit Date   : %s\n", depositDate);
    printf("Country        : %s\n", country);
    printf("Phone Number   : %s\n", phone);
    printf("Balance        : $%.2f\n", balance);
    printf("Account Type   : %s\n", accountType);
    printf("═══════════════════════════════════════════════════════════════\n");

    printf("\n🚨 CRITICAL WARNING:\n");
    printf("• This action will PERMANENTLY delete this account\n");
    printf("• Account balance of $%.2f will be LOST\n", balance);
    printf("• This action CANNOT be undone\n");
    printf("• All account history will be removed\n");
}

void showRemovalConfirmationMenu()
{
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Deletion Confirmation:\n");
    printf("[1] Proceed with deletion\n");
    printf("[2] Cancel and return to main menu\n");
    printf("═══════════════════════════════════════════════════════════════\n");
}

void showRemovalSuccess(int accountId, double balance)
{
    printf("\t\t\t===== Account Deletion Complete =====\n");
    printf("\n✔ SUCCESS: Account has been permanently deleted!\n");
    printf("\n═══════════════ Deletion Summary ═══════════════\n");
    printf("Deleted Account ID: %d\n", accountId);
    printf("Previous Balance  : $%.2f\n", balance);
    printf("Status           : Permanently Removed\n");
    printf("═══════════════════════════════════════════════\n");
}

// =====================================================
// TRANSFER DISPLAY FUNCTIONS
// =====================================================

void showTransferHeader()
{
    printf("\t\t\t===== Transfer Account Ownership =====\n");
}

void showTransferAccountDisplay(int accountId, const char *accountType, double balance)
{
    printf("\t\t\t===== Transfer Account Ownership =====\n");
    printf("\n✔ Selected Account ID: %d (%s)\n", accountId, accountType);
    printf("Current Balance: $%.2f\n", balance);

    // Get target username with validation
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Transfer Recipient Information:\n");
    printf("═══════════════════════════════════════════════════════════════\n");
}
void showTransferConfirmation(int accountId, const char *accountType, double balance,
                              const char *country, const char *phone, const char *depositDate,
                              const char *fromUser, const char *toUser)
{
    printf("\t\t\t===== Transfer Account Ownership =====\n");
    printf("\n⚠️  OWNERSHIP TRANSFER CONFIRMATION\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("ACCOUNT TO TRANSFER:\n");
    printf("Account Number : %d\n", accountId);
    printf("Account Type   : %s\n", accountType);
    printf("Balance        : $%.2f\n", balance);
    printf("Country        : %s\n", country);
    printf("Phone Number   : %s\n", phone);
    printf("Deposit Date   : %s\n", depositDate);
    printf("───────────────────────────────────────────────────────────────\n");
    printf("TRANSFER DETAILS:\n");
    printf("From           : %s (You)\n", fromUser);
    printf("To             : %s\n", toUser);
    printf("═══════════════════════════════════════════════════════════════\n");

    printf("\n🚨 IMPORTANT WARNINGS:\n");
    printf("• You will LOSE complete access to this account\n");
    printf("• %s will become the new owner\n", toUser);
    printf("• This transfer is PERMANENT and cannot be undone\n");
    printf("• All future transactions will require the new owner's authorization\n");
}

void showTransferConfirmationMenu()
{
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Transfer Confirmation:\n");
    printf("[1] Proceed with ownership transfer\n");
    printf("[2] Cancel and return to main menu\n");
    printf("═══════════════════════════════════════════════════════════════\n");
}

void showTransferSuccess(int accountId, const char *accountType, double balance,
                         const char *fromUser, const char *toUser)
{
    printf("\t\t\t===== Transfer Complete =====\n");
    printf("\n✔ SUCCESS: Ownership transfer completed!\n");
    printf("\n═══════════════ Transfer Summary ═══════════════\n");
    printf("Transferred Account: %d (%s)\n", accountId, accountType);
    printf("Account Balance    : $%.2f\n", balance);
    printf("Previous Owner     : %s\n", fromUser);
    printf("New Owner          : %s\n", toUser);
    printf("Transfer Status    : Completed Successfully\n");
    printf("═══════════════════════════════════════════════\n");
    printf("\nNOTE: You no longer have access to this account.\n");
    printf("%s is now the sole owner and can manage all account activities.\n", toUser);
}

// =====================================================
// INPUT PROMPT FUNCTIONS
// =====================================================

// void showInputPrompt(const char* prompt)
// {
//     printf("%s", prompt);
// }

void showRetryMessage()
{
    printf("✖ Invalid input! Please try again.\n");
    sleep(2);
}

void showInputErrorMessage()
{
    printf("✖ Input error! Please try again.\n");
    sleep(2);
}

// // void showDatabaseErrorMessage(struct User u, sqlite3 *db)
// // {
// //     printf("✖ Database error: %s\n", sqlite3_errmsg(db));
// //     sleep(2);
// //     mainMenu(u);
// //     return;
// // }

// void showInvalidOperationMessage()
// {
//     printf("✖ Invalid operation! Please choose a valid option.\n");
//     sleep(2);
// }

// // =====================================================
// // PROGRESS AND STATUS FUNCTIONS
// // =====================================================

void showDivider()
{
    printf("═══════════════════════════════════════════════════════════════\n");
}

void showShortDivider()
{
    printf("───────────────────────────────────────────────────────────────\n");
}

// =====================================================
// VALIDATION MESSAGE FUNCTIONS
// =====================================================

void showValidationError(const char *field, const char *requirement)
{
    printf("✖ Invalid %s! %s\n", field, requirement);
    sleep(2);
}

void showInvalidSelectionError(int maxCount)
{
    printf("✖ Invalid selection! Please choose a number between 1 and %d.\n", maxCount);
    sleep(2);
}

void showCancelledAction(const char *action, struct User u)
{
    printf("\n✔ Account %s cancelled. Returning to main menu...\n", action);
    sleep(2);
    mainMenu(u);
    return;
}

void showConfirmationInput(const char *action)
{
    printf("✖ Confirmation failed! You must type '%s' exactly.\n", action);
    sleep(3);
}

// =====================================================
// ACCOUNT SELECTION FUNCTIONS
// ===============   ======================================

void showAccountSelectionHeader()
{
    printf("\nYour accounts:\n");
    showDivider();
}

void showAccountItemWithDetails(int itemNumber, int accountId, const char *accountType,
                                double balance, const char *country, const char *phone)
{
    printf("[%d] Account ID: %d (%s)\n", itemNumber, accountId, accountType);
    printf("    Current Balance: $%.2f\n", balance);
    printf("    Country: %s | Phone: %s\n", country, phone);
    showShortDivider();
}