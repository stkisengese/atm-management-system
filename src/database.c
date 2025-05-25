#include "header.h"

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