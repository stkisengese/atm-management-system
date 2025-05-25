#include <termios.h>
#include "header.h"
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>

void loginMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
    scanf("%49s", a);

    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        exit(1);
    }
    printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
    scanf("%49s", pass);

    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        exit(1);
    }
}

void registerMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;
    char confirmPass[50];

    // ---------- Loop for Valid Username ----------
    while (1)
    {
        system("clear");
        printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Registration:");

        printf("\n\t\t\t\tEnter username: ");
        scanf("%49s", a);

        if (!validateName(a))
        {
            sleep(2);
            continue;
        }

        break; // username is valid
    }

    // ---------- Loop for Valid Password ----------
    while (1)
    {
        // Disable echo for secure password input
        tcgetattr(fileno(stdin), &oflags);
        nflags = oflags;
        nflags.c_lflag &= ~ECHO;
        nflags.c_lflag |= ECHONL;

        if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
        {
            perror("tcsetattr");
            exit(1);
        }

        printf("\n\t\t\t\tEnter password: ");
        scanf("%49s", pass);

        if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
        {
            perror("tcsetattr");
            exit(1);
        }

        if (!validatePassword(pass))
        {
            sleep(2);
            continue;
        }

        // ---------- Confirm Password ----------
        // Disable echo again
        tcsetattr(fileno(stdin), TCSANOW, &nflags);
        printf("\n\t\t\t\tRe-enter password: ");
        scanf("%49s", confirmPass);
        tcsetattr(fileno(stdin), TCSANOW, &oflags);

        if (strcmp(pass, confirmPass) != 0)
        {
            printf("\n\t\t\t\t✖ Passwords do not match. Try again.\n");
            sleep(2);
            continue;
        }

        break; // password is valid and confirmed
    }
}

int getPassword(struct User *u)
{
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;

    char sql[] = "SELECT id, password FROM users WHERE name = ?";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, u->name, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        u->id = sqlite3_column_int(stmt, 0);
        strcpy(u->password, (char *)sqlite3_column_text(stmt, 1));
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

int registerUser(char name[50], char password[50])
{
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;

    // Check if user already exists
    char check_sql[] = "SELECT COUNT(*) FROM users WHERE name = ?";
    int rc = sqlite3_prepare_v2(db, check_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0)
    {
        sqlite3_finalize(stmt);
        return 0; // User already exists
    }
    sqlite3_finalize(stmt);

    // Insert new user
    char insert_sql[] = "INSERT INTO users (name, password) VALUES (?, ?)";
    rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? 1 : 0;
}

int validatePassword(const char *pass)
{
    if (!pass)
    {
        printf("\t\t\t\t✖ Password cannot be empty.\n");
        return 0;
    }

    size_t len = strlen(pass);

    if (len < 4 || len > 12)
    {
        printf("\t\t\t\t✖ Password must be between 4 and 12 characters long.\n");
        return 0;
    }

    for (size_t i = 0; i < len; i++)
    {
        char c = pass[i];
        if (!(isalnum((unsigned char)c) || c == '-' || c == '_'))
        {
            printf("\t\t\t\t✖ Password can only contain letters, numbers, '-' and '_'.\n");
            return 0;
        }
    }

    return 1; // ✅ Valid
}
