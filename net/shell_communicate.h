#ifndef SHELL_COMMUNICATE_H
#define SHELL_COMMUNICATE_H

void request_shell(int socket);
void send_command(int socket);
void terminate_session(int socket);

#endif //SHELL_COMMUNICATE_H