#ifndef ACCOUNT_DB_H
#define ACCOUNT_DB_H

#define MAX_USERS 10

typedef struct {
    char username[50];
    char password[50];
} UserAccount;

extern const int user_count;
extern UserAccount user_accounts[MAX_USERS];

// UserAccount user_accounts[MAX_USERS] = {
//     {"AAA", "aaa"},
//     {"BBB", "bbb"},
//     {"CCC", "ccc"},
//     {"test", "test"}

// };

// const int user_count = 4;//= sizeof(user_accounts) / sizeof(UserAccount);

#endif //ACCOUNT_DB_H