#include "header.h"
#include <unistd.h>
#include <stdio.h>

void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
    int option;
    if (notGood == 0)
    {
        system("clear");
        printf("\n✖ Record not found!!\n");
    invalid:
        printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit:");
        scanf("%d", &option);
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
            printf("Insert a valid operation!\n");
            sleep(2);
            goto invalid;
        }
    }
    else
    {
        printf("\nEnter 1 to go to the main menu and 0 to exit:");
        scanf("%d", &option);
    }
    if (option == 1)
    {
        system("clear");
        mainMenu(u);
    }
    else
    {
        system("clear");
        printf("Thank you for using our ATM system!\n");
        closeDatabase();
        exit(0);
    }
}

void success(struct User u)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    scanf("%d", &option);
    system("clear");
    if (option == 1)
    {
        mainMenu(u);
    }
    else if (option == 0)
    {
        printf("Thank you for using our ATM system!\n");
        closeDatabase();
        exit(0);
    }
    else
    {
        printf("Insert a valid operation!\n");
        sleep(2);
        goto invalid;
    }
}

void createNewAcc(struct User u)
{
    struct Record r;
    sqlite3 *db = getDatabase();
    sqlite3_stmt *stmt;

noAccount:
    system("clear");
    printf("\t\t\t===== New record =====\n");

    printf("\nEnter today's date(mm/dd/yyyy):");
    scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year);
    printf("\nEnter the account number:");
    scanf("%d", &r.accountNbr);

    // Check if account already exists for this user
    char check_sql[] = "SELECT COUNT(*) FROM records WHERE user_id = ? AND account_id = ?";
    int rc = sqlite3_prepare_v2(db, check_sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, u.id);
        sqlite3_bind_int(stmt, 2, r.accountNbr);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0)
        {
            printf("✖ This Account already exists for this user\n\n");
            sqlite3_finalize(stmt);
            sleep(2);
            goto noAccount;
        }
        sqlite3_finalize(stmt);
    }

    printf("\nEnter the country:");
    scanf("%s", r.country);
    printf("\nEnter the phone number:");
    scanf("%d", &r.phone);
    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);
    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice:");
    scanf("%s", r.accountType);

    // Insert new account record
    char insert_sql[] = "INSERT INTO records (user_id, user_name, account_id, deposit_date, country, phone, balance, account_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL);

    if (rc == SQLITE_OK)
    {
        char date_str[20];
        sprintf(date_str, "%d/%d/%d", r.deposit.month, r.deposit.day, r.deposit.year);

        sqlite3_bind_int(stmt, 1, u.id);
        sqlite3_bind_text(stmt, 2, u.name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, r.accountNbr);
        sqlite3_bind_text(stmt, 4, date_str, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, r.country, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 6, r.phone);
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
    printf("\t\t====== All accounts from user, %s =====\n\n", u.name);

    int found = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        found = 1;
        printf("_____________________\n");
        printf("\nAccount number: %d\n", sqlite3_column_int(stmt, 0));
        printf("Deposit Date    : %s\n", sqlite3_column_text(stmt, 1));
        printf("Country         : %s\n", sqlite3_column_text(stmt, 2));
        printf("Phone number    : %s\n", sqlite3_column_text(stmt, 3));
        printf("Amount deposited: $%.2f\n", sqlite3_column_double(stmt, 4));
        printf("Type Of Account : %s\n", sqlite3_column_text(stmt, 5));
    }

    sqlite3_finalize(stmt);

    if (!found)
    {
        printf("No accounts found for user %s\n", u.name);
    }

    success(u);
}