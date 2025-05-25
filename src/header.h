#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

// database functions
int initDatabase();
sqlite3 *getDatabase();
void closeDatabase();
int accountExists(int accountId);
int validateDate(int month, int day, int year);
int validateAccountType(char *accountType);
int validateName(char *name);
int validatePhone(const char *phoneStr);
int validateAmount(const char *amountStr);
int validateAccountNumber(const char *acctNum);

// authentication functions
void loginMenu(char a[50], char pass[50]);
void registerMenu(char a[50], char pass[50]);
int getPassword(struct User *u);
int registerUser(char name[50], char password[50]);
int validatePassword(const char *pass);

// system function
void createNewAcc(struct User u);
void mainMenu(struct User u);
void checkAllAccounts(struct User u);

#endif // HEADER_H