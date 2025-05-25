#include "header.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static sqlite3 *db = NULL;

int initDatabase()
{
    int rc = sqlite3_open("atm.db", &db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Create users table
    char *sql_users = "CREATE TABLE IF NOT EXISTS users ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT UNIQUE NOT NULL CHECK(LENGTH(name) > 2 AND name NOT GLOB '*[0-9]*'),"
                      "password TEXT NOT NULL CHECK(LENGTH(password) >= 4 AND LENGTH(password) <= 49));";

    rc = sqlite3_exec(db, sql_users, 0, 0, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error creating users table: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Create records table
    char *sql_records = "CREATE TABLE IF NOT EXISTS records ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "user_id INTEGER NOT NULL,"
                        "user_name TEXT NOT NULL,"
                        "account_id INTEGER NOT NULL,"
                        "deposit_date TEXT NOT NULL CHECK(deposit_date GLOB '[0-9][0-9]/[0-9][0-9]/[0-9][0-9][0-9][0-9]'),"
                        "country TEXT NOT NULL CHECK(LENGTH(country) > 0 AND country NOT GLOB '*[0-9]*'),"
                        "phone TEXT NOT NULL CHECK(phone GLOB '+[0-9]*' OR phone GLOB '0[0-9]*'),"
                        "balance REAL NOT NULL CHECK(balance >= 0),"
                        "account_type TEXT NOT NULL CHECK(account_type IN ('saving', 'current', 'fixed01', 'fixed02', 'fixed03')),"
                        "UNIQUE(user_id, account_id),"
                        "FOREIGN KEY(user_id) REFERENCES users(id));";

    rc = sqlite3_exec(db, sql_records, 0, 0, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error creating records table: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    return 0;
}

sqlite3 *getDatabase()
{
    return db;
}

void closeDatabase()
{
    if (db)
    {
        sqlite3_close(db);
        db = NULL;
    }
}

// Check if account exists for a specific user
int accountExists(int accountId)
{
    sqlite3 *database = getDatabase();
    sqlite3_stmt *stmt;

    char sql[] = "SELECT COUNT(*) FROM records WHERE account_id = ?";

    int rc = sqlite3_prepare_v2(database, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare account check statement: %s\n", sqlite3_errmsg(database));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, accountId);

    rc = sqlite3_step(stmt);
    int exists = 0;
    if (rc == SQLITE_ROW)
    {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    return exists;
}

// Validate date format and values
int validateDate(int month, int day, int year)
{
    if (year < 1900 || year > 2100)
    {
        printf("✖ Year must be between 1900 and 2100\n");
        return 0;
    }

    if (month < 1 || month > 12)
    {
        printf("✖ Month must be between 1 and 12\n");
        return 0;
    }

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Adjust for leap year
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
    {
        daysInMonth[1] = 29;
    }

    if (day < 1 || day > daysInMonth[month - 1])
    {
        printf("✖ Day %d is invalid for month %d\n", day, month);
        return 0;
    }

    return 1; // ✅ All checks passed
}

// Validate account type
int validateAccountType(char *accountType)
{
    if (!accountType || strlen(accountType) == 0)
    {
        printf("✖ Account type cannot be empty\n");
        return 0;
    }

    char validTypes[][10] = {"saving", "current", "fixed01", "fixed02", "fixed03"};
    int numTypes = sizeof(validTypes) / sizeof(validTypes[0]);

    for (int i = 0; i < numTypes; i++)
    {
        if (strcmp(accountType, validTypes[i]) == 0)
        {
            return 1;
        }
    }
    printf("✖ Invalid account type");
    return 0;
}

// Validate name (no numbers, not empty)
int validateName(char *name)
{
    if (!name || strlen(name) == 0)
    {
        printf("✖ Name cannot be null or empty\n");
        return 0;
    }

    size_t len = strlen(name);
    if (len < 2 || len > 26)
    {
        printf("✖ Name must be between 2 and 26 characters\n");
        return 0;
    }

    // Must start with a letter
    if (!isalpha((unsigned char)name[0]))
    {
        printf("✖ Name must start with a letter\n");
        return 0;
    }

    for (size_t i = 0; i < len; i++)
    {
        char c = name[i];
        // Allow letters, spaces, hyphens, underscores only
        if (!(isalpha((unsigned char)c) || c == ' ' || c == '-' || c == '_'))
        {
            printf("✖ Invalid character '%c' in name\n", c);
            return 0;
        }
    }

    return 1; // Name is valid
}

// Validates local and international-style phone numbers
int validatePhone(const char *phoneStr)
{
    size_t len = strlen(phoneStr);

    if (len < 10 || len > 15)
    {
        printf("✖ Phone number must be between 10 and 15 digits long.\n");
        return 0;
    }

    int start = 0;

    // Allow optional leading '+' (but skip it in digit checks)
    if (phoneStr[0] == '+')
    {
        start = 1;

        if (len < 11 || len > 15)
        { // + plus 10+ digits
            printf("✖ International number must have 10 to 14 digits after '+'.\n");
            return 0;
        }
    }

    // Check that the rest are all digits
    for (size_t i = start; i < len; i++)
    {
        if (!isdigit((unsigned char)phoneStr[i]))
        {
            printf("✖ Phone number must contain digits only%s.\n", start ? " after the '+'" : "");
            return 0;
        }
    }

    return 1; // Valid
}

// Validate amount string (only digits and decimal point)
int validateAmount(const char *amountStr)
{
    if (!amountStr || strlen(amountStr) == 0)
    {
        printf("✖ Amount cannot be empty.\n");
        return 0;
    }

    int dotSeen = 0;

    for (size_t i = 0; amountStr[i] != '\0'; i++)
    {
        if (amountStr[i] == '.')
        {
            if (dotSeen)
            {
                printf("✖ Amount cannot contain more than one decimal point.\n");
                return 0;
            }
            dotSeen = 1;
        }
        else if (!isdigit((unsigned char)amountStr[i]))
        {
            printf("✖ Amount must contain digits only (e.g., 100 or 99.50).\n");
            return 0;
        }
    }

    double value = atof(amountStr);
    if (value <= 0)
    {
        printf("✖ Amount must be greater than zero.\n");
        return 0;
    }

    return 1; // Valid
}

// ValidateAccountNumber to digits only, positive intenger and 6-12 digits
int validateAccountNumber(const char *acctNum)
{
    size_t len = strlen(acctNum);

    if (len < 6 || len > 12)
    {
        printf("✖ Account number must be between 6 and 12 digits long.\n");
        return 0;
    }

    for (size_t i = 0; i < len; i++)
    {
        if (!isdigit((unsigned char)acctNum[i]))
        {
            printf("✖ Account number must contain digits only.\n");
            return 0;
        }
    }

    // we reject account numbers starting with 0
    if (acctNum[0] == '0')
    {
        printf("✖ Account number cannot start with 0.\n");
        return 0;
    }

    return 1; // Valid
}