#ifndef ACCOUNT_DB_H
#define ACCOUNT_DB_H

#define MAX_USERS 10

const int user_count = 4;//= sizeof(user_accounts) / sizeof(UserAccount);

typedef struct {
    char username[50];
    char password[50];
} UserAccount;

UserAccount user_accounts[MAX_USERS] = {
    {"AAA", "aaa"},
    {"BBB", "bbb"},
    {"CCC", "ccc"},
    {"test", "test"}

};

#endif //ACCOUNT_DB_H