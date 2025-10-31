#ifndef AUTHEN_H
#define AUTHEN_H

#include "account_db.h"
#include "telnet.h"

void authenticate(int socket);
int validate_credentials(const char *username, const char *password);
void handle_client(int socket);

#endif //AUTHEN_H