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
    
    // --- 1. AUTHENTICATION ---
    // The server will send commands first. 
    // telnet_read_prompt will handle them automatically.
    
    char username[BUFFER_SIZE], password[BUFFER_SIZE];
    char server_prompt[BUFFER_SIZE];

    // Read "Username: " prompt (10 bytes)
    // Use telnet_read_prompt, which will handle negotiations.
    if (telnet_read_prompt(sock, server_prompt, 10) <= 0) {
        fprintf(stderr, "Server disconnected during authentication\n");
        close(sock);
        return 1;
    }
    printf("%s", server_prompt); // Print the prompt
    
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Strip newline
    telnet_writeln(sock, username); // Send with \r\n

    // Read "Password: " prompt (10 bytes)
    if (telnet_read_prompt(sock, server_prompt, 10) <= 0) {
        fprintf(stderr, "Server disconnected during auth\n");
        close(sock);
        return 1;
    }
    printf("%s", server_prompt); // Print the prompt
    
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Strip newline
    telnet_writeln(sock, password); // Send with \r\n

    // Read auth response (this IS a line)
    char auth_response[BUFFER_SIZE];
    if (telnet_read_line(sock, auth_response, BUFFER_SIZE) <= 0) {
        fprintf(stderr, "Server disconnected during auth response\n");
        close(sock);
        return 1;
    }
    printf("Server: %s\n", auth_response); // telnet_read_line stripped \n

    if (strncmp(auth_response, "Authentication failed", 21) == 0) {
        close(sock);
        return 0;
    }

    // --- 2. MESSAGE EXCHANGE ---
    while(1) {
        // Read the server's "> " prompt (2 bytes)
        if (telnet_read_prompt(sock, server_prompt, 2) <= 0) {
            fprintf(stderr, "Server disconnected\n");
            break;
        }
        printf("%s", server_prompt); // Print the "> "
        
        // Get input from user
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Strip newline
        telnet_writeln(sock, buffer); // Send line with \r\n

        // Read the server's response (this IS a line)
        char response_text[BUFFER_SIZE];
        if (telnet_read_line(sock, response_text, BUFFER_SIZE) <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        
        printf("Server: %s\n", response_text);

        if (strcmp(buffer, "quit") == 0) {
            break;
        }
    }
    close(sock);
    return 0;
}
