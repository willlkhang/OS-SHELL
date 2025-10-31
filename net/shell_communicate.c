#include "shell_communicate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#define BUFFER_SIZE 1024

#include "telnet.h"

void request_shell(int socket) {
    char opcode = 'B';
    int shell_choice = 0;

    // Prompt client to choose shell type
    printf("Select shell type:\n");
    printf("1. Bash Shell.\n");
    printf("2. MyRunShell.\n");
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

    printf("%% ");
    fflush(stdout);
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
