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
