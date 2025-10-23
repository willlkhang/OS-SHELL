#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "stream.h"

#define PORT 65432
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *server_ip = argv[1];

    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // Use the provided IP address
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton: Invalid IP address");
        exit(EXIT_FAILURE);
    }
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server at %s.\n", server_ip);
    
    char username[BUFFER_SIZE], password[BUFFER_SIZE];
    
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    uint8_t ulen = strlen(username);
    uint8_t plen = strlen(password);
    
    size_t auth_msg_size = 1 + 1 + ulen + 1 + plen;
    char *auth_msg = malloc(auth_msg_size);
    auth_msg[0] = 'A';
    auth_msg[1] = ulen;
    memcpy(auth_msg + 2, username, ulen);
    auth_msg[2 + ulen] = plen;
    memcpy(auth_msg + 3 + ulen, password, plen);

    write_all(sock, auth_msg, auth_msg_size);
    free(auth_msg);
    
    char auth_response[2];
    if (read_all(sock, auth_response, 2) != 0 || auth_response[0] != 'R') {
        fprintf(stderr, "Failed to get valid authentication response.\n");
        close(sock);
        return 1;
    }
    
    if (auth_response[1] == '1') {
        printf("Incorrect username or password.\n");
        close(sock);
        return 0;
    }
    
    printf("Authentication successful!\n");
    while(1) {
        printf("> ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        
        uint16_t msg_len = strlen(buffer);
        uint16_t msg_len_net = htons(msg_len);
        
        char client_opcode = 'M';
        write_all(sock, &client_opcode, 1);
        write_all(sock, &msg_len_net, sizeof(uint16_t));
        write_all(sock, buffer, msg_len);

        char server_opcode;
        if (read_all(sock, &server_opcode, 1) != 0 || server_opcode != 'S') {
            fprintf(stderr, "Connection lost or invalid response from server.\n");
            break;
        }

        uint16_t response_len_net;
        read_all(sock, &response_len_net, sizeof(uint16_t));
        uint16_t response_len = ntohs(response_len_net);

        char response_text[BUFFER_SIZE];
        read_all(sock, response_text, response_len);
        response_text[response_len] = '\0';
        
        printf("Server: %s\n", response_text);

        if (strcmp(buffer, "quit") == 0) {
            break;
        }
    }
    
    close(sock);
    return 0;
}

