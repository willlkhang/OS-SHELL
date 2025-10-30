#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define SERVER_PORT 40338
#define BUFFER_SIZE 1024
#define MAX_USERS 10

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
int user_count = 4;//= sizeof(user_accounts) / sizeof(UserAccount);

void handle_client(int client_socket);
int validate_credentials(const char *username, const char *password);
void handle_sigchld(int sig);

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);

    printf("Server is listening on port %d\n", SERVER_PORT);

    signal(SIGCHLD, handle_sigchld);

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Server accept failed");
            continue;
        }

        if (fork() == 0) {
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        }
        close(client_socket);
    }

    close(server_socket);
    return 0;
}

void handle_client(int socket) {
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

int validate_credentials(const char *username, const char *password) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_accounts[i].username, username) == 0 &&
            strcmp(user_accounts[i].password, password) == 0) {
            return 1;
        }
    }
    return 0;
}

void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

