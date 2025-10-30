// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#define SERVER_PORT 40338
#define BUFFER_SIZE 1024

// Helpers
ssize_t send_all(int sock, const void *buf, size_t len) {
    const uint8_t *p = buf;
    size_t remaining = len;
    while (remaining > 0) {
        ssize_t sent = send(sock, p, remaining, 0);
        if (sent <= 0) {
            if (sent < 0 && errno == EINTR) continue;
            return -1;
        }
        p += sent;
        remaining -= sent;
    }
    return (ssize_t)len;
}

ssize_t recv_all(int sock, void *buf, size_t len) {
    uint8_t *p = buf;
    size_t remaining = len;
    while (remaining > 0) {
        ssize_t r = recv(sock, p, remaining, 0);
        if (r <= 0) {
            if (r < 0 && errno == EINTR) continue;
            return -1;
        }
        p += r;
        remaining -= r;
    }
    return (ssize_t)len;
}

// Function prototypes (same interface as your program)
void authenticate(int socket);
void request_shell(int socket);
void send_command(int socket);
void terminate_session(int socket);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *server_ip = argv[1];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }

    authenticate(sock);
    request_shell(sock);

    // main loop
    while (1) {
        send_command(sock);
    }

    close(sock);
    return 0;
}

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

void request_shell(int socket) {
    char opcode = 'B';
    int shell_choice = 0;

    // Prompt client to choose shell type
    printf("Select shell type:\n");
    printf("1. Standard shell (bash)\n");
    printf("2. Custom shell\n");
    printf("Enter your choice (1 or 2): ");
    if (scanf("%d", &shell_choice) != 1) {
        fprintf(stderr, "Invalid input\n");
        close(socket);
        exit(EXIT_FAILURE);
    }
    if (shell_choice != 1 && shell_choice != 2) {
        fprintf(stderr, "Invalid choice. Using 1 (bash) by default.\n");
        shell_choice = 1;
    }

    // Send opcode then raw int (server expects sizeof(int) bytes as in your server)
    if (send_all(socket, &opcode, 1) < 0 ||
        send_all(socket, &shell_choice, sizeof(shell_choice)) < 0) {
        perror("send");
        close(socket);
        exit(EXIT_FAILURE);
    }

    char ack_opcode = 0, ack_code = 0;
    if (recv_all(socket, &ack_opcode, 1) < 0 || recv_all(socket, &ack_code, 1) < 0) {
        fprintf(stderr, "Failed to receive shell ack\n");
        close(socket);
        exit(EXIT_FAILURE);
    }

    if (ack_code == '0') {
        printf("Shell session started successfully.\n");
    } else {
        fprintf(stderr, "Shell session initiation failed (code: %c).\n", ack_code);
        close(socket);
        exit(EXIT_FAILURE);
    }
}

void send_command(int socket) {
    char opcode = 'C';
    char command[BUFFER_SIZE];

    printf("Enter command (or 'exit' to quit): ");
    // read a full line including spaces
    if (scanf(" %[^\n]", command) != 1) {
        fprintf(stderr, "Failed to read command\n");
        return;
    }

    if (strcmp(command, "exit") == 0) {
        terminate_session(socket);
        exit(0);
    }

    uint16_t cmd_len_net;
    uint16_t cmd_len = (uint16_t)strlen(command);
    if (cmd_len == 0) {
        printf("Empty command, skipping.\n");
        return;
    }

    cmd_len_net = htons(cmd_len);

    // Send opcode, length (network order), and command bytes
    if (send_all(socket, &opcode, 1) < 0 ||
        send_all(socket, &cmd_len_net, sizeof(cmd_len_net)) < 0 ||
        send_all(socket, command, cmd_len) < 0) {
        perror("send");
        close(socket);
        exit(EXIT_FAILURE);
    }

    // Read response opcode (1 byte) then 4 byte length
    char response_opcode = 0;
    uint32_t output_len_net = 0;
    if (recv_all(socket, &response_opcode, 1) < 0 ||
        recv_all(socket, &output_len_net, sizeof(output_len_net)) < 0) {
        fprintf(stderr, "Failed to read command response header\n");
        close(socket);
        exit(EXIT_FAILURE);
    }

    uint32_t output_len = ntohl(output_len_net);
    if (output_len > 10 * 1024 * 1024) { // arbitrary safety limit (10 MB)
        fprintf(stderr, "Output length too large (%u). Aborting.\n", output_len);
        close(socket);
        exit(EXIT_FAILURE);
    }

    char *output = malloc(output_len + 1);
    if (!output) {
        fprintf(stderr, "Out of memory\n");
        close(socket);
        exit(EXIT_FAILURE);
    }

    if (recv_all(socket, output, output_len) < 0) {
        fprintf(stderr, "Failed to read command output\n");
        free(output);
        close(socket);
        exit(EXIT_FAILURE);
    }
    output[output_len] = '\0';

    printf("Command Output:\n%s\n", output);
    free(output);
}

void terminate_session(int socket) {
    char opcode = 'D';

    if (send_all(socket, &opcode, 1) < 0) {
        perror("send");
        close(socket);
        exit(EXIT_FAILURE);
    }

    char ack_opcode = 0, ack_code = 0;
    if (recv_all(socket, &ack_opcode, 1) < 0 || recv_all(socket, &ack_code, 1) < 0) {
        fprintf(stderr, "Failed to receive termination ack\n");
        close(socket);
        exit(EXIT_FAILURE);
    }

    if (ack_code == '0') {
        printf("Session terminated successfully.\n");
    } else {
        printf("Session termination failed (code: %c)\n", ack_code);
    }
}
