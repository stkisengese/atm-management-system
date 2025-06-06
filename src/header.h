#define _GNU_SOURCE
#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <limits.h>
#include <ctype.h>
#include "../sqlite/sqlite3.h"

// Date structure
struct Date
{
    int month, day, year;
};

// all fields for each record of an account
struct Record
{
    int id;
    int userId;
    char name[100];
    char country[100];
    char phone[20];
    char accountType[10];
    int accountNbr;
    double amount;
    struct Date deposit;
    struct Date withdraw;
};

struct User
{
    int id;
    char name[50];
    char password[50];
};

// Input handling functions
void clearInputBuffer();
int safeStringInput(char *buffer, int size, const char *prompt);
int safeIntInput(int *value, const char *prompt);
void handleLogout(struct User u);
int getMenuChoice(const char *prompt, int min, int max);

// database functions
int initDatabase();
sqlite3 *getDatabase();
void closeDatabase();
int accountExists(int accountId);
int validateDate(int month, int day, int year);
int validateName(char *name);
int validatePhone(const char *phoneStr);
int validateAmount(const char *amountStr);
int validateAccountNumber(const char *acctNum);

// authentication functions
void initMenu(struct User *u);
void mainMenu(struct User u);
void loginMenu(char a[50], char pass[50]);
void registerMenu(char a[50], char pass[50]);
int getPassword(struct User *u);
int registerUser(char name[50], char password[50]);
int validatePassword(const char *pass);
int authenticateUser(struct User *u, char *inputPass);
int registerNewUser(struct User *u, char *temp_name, char *temp_pass);

// system function
void createNewAcc(struct User u);
void mainMenu(struct User u);
void checkAllAccounts(struct User u);

// transaction functions
void updateAccountInfo(struct User u);
void checkAccountDetails(struct User u);
void makeTransaction(struct User u);
void removeAccount(struct User u);
void checkAccountDetails(struct User u);
void transferOwnership(struct User u);
int isFixedAccount(const char *accountType);
void displayInterestInfo(const char *accountType, double balance, const char *depositDate);

// =====================================================
// GENERAL DISPLAY FUNCTIONS
// =====================================================
void clearScreen();
void showATMHeader();
void showErrorMessage(const char *message);
void showInputErrorMessage();
void showThankYouMessage();

// =====================================================
// MENU DISPLAY FUNCTIONS
// =====================================================
void showMainMenu(struct User u);
void showLoginMenu();
void showAccountTypeMenu();
void showTransactionMenu(int selectedAccountId, const char *accountType, double currentBalance);
void showUpdateMenu(int selectedAccountId, const char *country, const char *phoneNumber);
void showUpdateDetails(const char *country, const char *phone);

// =====================================================
// ACCOUNT DISPLAY FUNCTIONS
// =====================================================
void showAccountCreationHeader();
void showAccountTypeInfo(int accountTypeChoice);
void showAccountPortfolioHeader(const char *userName);
void showAccountListItem(int itemNumber, int accountId, const char *accountType,
                         const char *depositDate, const char *country,
                         const char *phone, double balance);
void showPortfolioSummary(int totalAccounts, double totalBalance);
void showNoAccountsMessage(const char *userName);
void showAccountDetails(int accountId, const char *depositDate, const char *country,
                        const char *phone, double balance, const char *accountType);

// =====================================================
// TRANSACTION DISPLAY FUNCTIONS
// =====================================================
void showTransactionSummary(int accountId, const char *transactionType,
                            double amount, double currentBalance, double newBalance);
void showTransactionResult(int accountId, const char *transactionType, double amount, double newBalance);

// // =====================================================
// // REMOVAL DISPLAY FUNCTIONS
// // =====================================================
void showRemovalHeader();
void showRemovalWarning(int accountId, const char *depositDate, const char *country,
                        const char *phone, double balance, const char *accountType);
void showRemovalConfirmationMenu();
void showRemovalSuccess(int accountId, double balance);

// =====================================================
// TRANSFER DISPLAY FUNCTIONS
// =====================================================
void showTransferHeader();
void showTransferAccountDisplay(int accountId, const char *accountType, double balance);
void showTransferConfirmation(int accountId, const char *accountType, double balance,
                              const char *country, const char *phone, const char *depositDate,
                              const char *fromUser, const char *toUser);
void showTransferConfirmationMenu();
void showTransferSuccess(int accountId, const char *accountType, double balance,
                         const char *fromUser, const char *toUser);

// =====================================================
// PROGRESS AND STATUS FUNCTIONS
// =====================================================
void showDivider();
void showShortDivider();

// =====================================================
// VALIDATION MESSAGE FUNCTIONS
// =====================================================
void showValidationError(const char *field, const char *requirement);
void showInvalidSelectionError(int maxCount);
void showCancelledAction(const char *action, struct User u);
void showConfirmationInput(const char *action);

// =====================================================
// ACCOUNT SELECTION FUNCTIONS
// =====================================================
void showAccountSelectionHeader();
void showAccountItemWithDetails(int itemNumber, int accountId, const char *accountType,
                                double balance, const char *country, const char *phone);

#endif // HEADER_H