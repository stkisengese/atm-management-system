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

// database functions
int initDatabase();
sqlite3 *getDatabase();
void closeDatabase();
int accountExists(int accountId);
int validateDate(int month, int day, int year);
// int validateAccountType(char *accountType);
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
// void showWelcomeMessage();
void showSuccessMessage();
void showErrorMessage(const char* message);
void showWarningMessage(const char* message);
void showInfoMessage(const char* message);
void showThankYouMessage();

// =====================================================
// MENU DISPLAY FUNCTIONS
// =====================================================
void showMainMenu(struct User u);
void showLoginMenu();
void showAccountTypeMenu();
// void showTransactionMenu();
// void showUpdateMenu();

// =====================================================
// ACCOUNT DISPLAY FUNCTIONS
// =====================================================
void showAccountCreationHeader();
void showAccountTypeInfo(int accountTypeChoice);
// void showAccountPortfolioHeader(const char* userName);
// void showAccountListItem(int itemNumber, int accountId, const char* accountType, 
//                         const char* depositDate, const char* country, 
//                         const char* phone, double balance);
// void showPortfolioSummary(int totalAccounts, double totalBalance);
// void showNoAccountsMessage(const char* userName);
// void showAccountDetails(int accountId, const char* depositDate, const char* country,
//                        const char* phone, double balance, const char* accountType);

// // =====================================================
// // TRANSACTION DISPLAY FUNCTIONS
// // =====================================================
// void showTransactionHeader();
// void showSelectedAccount(int accountId, const char* accountType, double balance);
// void showTransactionSummary(int accountId, const char* transactionType, 
//                            double amount, double currentBalance, double newBalance);
// void showTransactionResult(int accountId, const char* transactionType, double amount, double newBalance);

// // =====================================================
// // UPDATE DISPLAY FUNCTIONS
// // =====================================================
// void showUpdateHeader();
// void showUpdateProgress(const char* field, const char* newValue);

// // =====================================================
// // REMOVAL DISPLAY FUNCTIONS
// // =====================================================
// void showRemovalHeader();
// void showRemovalWarning(int accountId, const char* depositDate, const char* country,
//                        const char* phone, double balance, const char* accountType);
// void showRemovalConfirmationMenu();
// void showRemovalSuccess(int accountId, double balance);

// // =====================================================
// // TRANSFER DISPLAY FUNCTIONS
// // =====================================================
// void showTransferHeader();
// void showTransferConfirmation(int accountId, const char* accountType, double balance,
//                              const char* country, const char* phone, const char* depositDate,
//                              const char* fromUser, const char* toUser);
// void showTransferConfirmationMenu();
// void showTransferSuccess(int accountId, const char* accountType, double balance,
//                         const char* fromUser, const char* toUser);

// =====================================================
// INPUT PROMPT FUNCTIONS
// =====================================================
// void showInputPrompt(const char* prompt);
void showRetryMessage();
void showInputErrorMessage();
// // void showDatabaseErrorMessage(struct User u, sqlite3 *db);
// void showInvalidOperationMessage();

// // =====================================================
// // PROGRESS AND STATUS FUNCTIONS
// // =====================================================
// void showProcessingMessage(const char* action);
// void showDivider();
// void showShortDivider();

// =====================================================
// VALIDATION MESSAGE FUNCTIONS
// =====================================================
void showValidationError(const char* field, const char* requirement);
// void showConfirmationPrompt(const char* action);

// // =====================================================
// // ACCOUNT SELECTION FUNCTIONS
// // =====================================================
// void showAccountSelectionHeader(const char* action);
// void showSimpleAccountItem(int itemNumber, int accountId, const char* accountType, double balance);
// void showAccountItemWithDetails(int itemNumber, int accountId, const char* accountType, 
//                                double balance, const char* country, const char* phone);

#endif // HEADER_H