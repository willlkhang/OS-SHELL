#include "authen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#include "account_db.h"

#define BUFFER_SIZE 1024

void authenticate(int socket) {
    char opcode = 'A';
    char username[50];
    char password[50];

    printf("Enter username: ");
    if (scanf("%49s", username) != 1) {
        fprintf(stderr, "Failed to read username\n");
        exit(EXIT_FAILURE);
    }
    printf("Enter password: ");
    if (scanf("%49s", password) != 1) {
        fprintf(stderr, "Failed to read password\n");
        exit(EXIT_FAILURE);
    }

    uint8_t user_len = (uint8_t)strlen(username);
    uint8_t pass_len = (uint8_t)strlen(password);

    // Send packet
    if (send_all(socket, &opcode, 1) < 0 ||
        send_all(socket, &user_len, 1) < 0 ||
        send_all(socket, username, user_len) < 0 ||
        send_all(socket, &pass_len, 1) < 0 ||
        send_all(socket, password, pass_len) < 0) {
        perror("send");
        close(socket);
        exit(EXIT_FAILURE);
    }

    // Expect two bytes: ack_opcode, ack_code
    char ack_opcode = 0, ack_code = 0;
    if (recv_all(socket, &ack_opcode, 1) < 0 || recv_all(socket, &ack_code, 1) < 0) {
        fprintf(stderr, "Failed to receive authentication response\n");
        close(socket);
        exit(EXIT_FAILURE);
    }

    if (ack_code == '0') {
        printf("Authentication successful.\n");
    } else {
        fprintf(stderr, "Authentication failed with code: %c\n", ack_code);
        close(socket);
        exit(EXIT_FAILURE);
    }
}

int validate_credentials(const char *username, const char *password) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_accounts[i].username, username) == 0 &&
            strcmp(user_accounts[i].password, password) == 0) {
            return 1;
        }
    }
    return 0;
}

void handle_client(int socket) {
    signal(SIGPIPE, SIG_IGN);

    char opcode, ack_code;
    uint8_t user_len, pass_len;
    char username[BUFFER_SIZE], password[BUFFER_SIZE];

    recv(socket, &opcode, 1, 0);
    recv(socket, &user_len, 1, 0);
    recv(socket, username, user_len, 0);
    username[user_len] = '\0';
    recv(socket, &pass_len, 1, 0);
    recv(socket, password, pass_len, 0);
    password[pass_len] = '\0';

    if (validate_credentials(username, password)) {
        ack_code = '0';
        send(socket, &opcode, 1, 0);
        send(socket, &ack_code, 1, 0);
    } else {
        ack_code = '1';
        send(socket, &opcode, 1, 0);
        send(socket, &ack_code, 1, 0);
        close(socket);
        return;
    }

    recv(socket, &opcode, 1, 0);
    int shell_choice;
    recv(socket, &shell_choice, sizeof(shell_choice), 0);

    if (opcode == 'B') {
        ack_code = '0';
        send(socket, &opcode, 1, 0);
        send(socket, &ack_code, 1, 0);
    } else {
        printf("Unexpected opcode received. Terminating connection.\n");
        close(socket);
        return;
    }

    int to_shell[2], from_shell[2];
    pipe(to_shell);
    pipe(from_shell);

    pid_t pid = fork();
    if (pid == 0) {

        close(socket);

        dup2(to_shell[0], STDIN_FILENO);
        dup2(from_shell[1], STDOUT_FILENO);
        dup2(from_shell[1], STDERR_FILENO);

        close(to_shell[1]);
        close(from_shell[0]);

        if (shell_choice == 1) {
            execl("/bin/bash", "bash", "--login", NULL);
        } else {
            execl("./../run", "run", NULL);
            perror("execl run failed");
        }
        perror("Failed to start shell");
        exit(1);
    } else if (pid > 0) {
        close(to_shell[0]);
        close(from_shell[1]);

        while (1) {
            recv(socket, &opcode, 1, 0);

            if (opcode == 'C') {
                char command[BUFFER_SIZE];
                char output[BUFFER_SIZE];
                uint16_t cmd_len;

                recv(socket, &cmd_len, 2, 0);
                cmd_len = ntohs(cmd_len);
                recv(socket, command, cmd_len, 0);
                command[cmd_len] = '\0';

                memset(output, 0, BUFFER_SIZE);

                    write(to_shell[1], command, strlen(command));
                    write(to_shell[1], "\n", 1);

                    fd_set readfds;
                    FD_ZERO(&readfds);
                    FD_SET(from_shell[0], &readfds);

                    struct timeval timeout;
                    timeout.tv_sec = 1;
                    timeout.tv_usec = 0;

                    int ready = select(from_shell[0] + 1, &readfds, NULL, NULL, &timeout);
                    if (ready > 0) {
                        int bytes_read = read(from_shell[0], output, BUFFER_SIZE - 1);
                        if (bytes_read > 0) {
                            output[bytes_read] = '\0';
                        } else {
                            strcpy(output, "Command executed successfully with no output.");
                        }
                    } else if (ready == 0) {
                        strcpy(output, "Command executed successfully with no output.");
                    } else {
                        strcpy(output, "Error reading shell output.");
                    }

                uint32_t output_len = htonl(strlen(output));
                char response_opcode = 'D';
                send(socket, &response_opcode, 1, 0);
                send(socket, &output_len, 4, 0);
                send(socket, output, strlen(output), 0);
            } else if (opcode == 'D') {
                ack_code = '0';
                send(socket, &opcode, 1, 0);
                send(socket, &ack_code, 1, 0);

                // Terminate shell and clean up
                kill(pid, SIGTERM);
                waitpid(pid, NULL, 0);
                close(to_shell[1]);
                close(from_shell[0]);
                close(socket);

                return;
            }
        }
    }
}