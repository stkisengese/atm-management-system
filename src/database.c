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
                      "name TEXT UNIQUE NOT NULL,"
                      "password TEXT NOT NULL);";

    rc = sqlite3_exec(db, sql_users, 0, 0, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error creating users table: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Create records table
    char *sql_records = "CREATE TABLE IF NOT EXISTS records ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "user_id INTEGER,"
                        "user_name TEXT,"
                        "account_id INTEGER,"
                        "deposit_date TEXT,"
                        "country TEXT,"
                        "phone INTEGER,"
                        "balance REAL,"
                        "account_type TEXT,"
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