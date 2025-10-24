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
    char server_prompt[BUFFER_SIZE]; //for reading prompts liek username:

    if (read_all(sock, server_prompt, 10) != 0) { //advoid deadlock
        fprintf(stderr, "Server disconnected during authentication\n");
        close(sock);
        return 1;
    }
    server_prompt[10] = '\0'; //sikipping enterkey
    printf("%s", server_prompt); // Print the prompt
    
    fgets(username, sizeof(username), stdin);
    write_all(sock, username, strlen(username)); //send to sever

    //sever waiting to read password from user
    if (read_all(sock, server_prompt, 10) != 0) { 
        fprintf(stderr, "Server disconnected during auth\n");
        close(sock);
        return 1;
    }
    server_prompt[10] = '\0'; // Null-terminate
    printf("%s", server_prompt); // Print the prompt
    
    // Get password from user
    fgets(password, sizeof(password), stdin);
    write_all(sock, password, strlen(password)); // Send it

    // Read auth response (this IS a line, so read_line is correct)
    char auth_response[BUFFER_SIZE];
    if (read_line(sock, auth_response, BUFFER_SIZE) <= 0) {
        fprintf(stderr, "Server disconnected during auth response\n");
        close(sock);
        return 1;
    }
    printf("Server: %s\n", auth_response);

    if (strncmp(auth_response, "Authentication failed", 21) == 0) {
        close(sock);
        return 0;
    }

    // --- 2. Message Exchange ---
    while(1) {
        // Read the server's "> " prompt (2 bytes)
        if (read_all(sock, server_prompt, 2) != 0) { // <-- FIX: Use read_all
            fprintf(stderr, "Server disconnected\n");
            break;
        }
        server_prompt[2] = '\0'; // Null-terminate
        printf("%s", server_prompt); // Print the "> "
        
        // Get input from user
        fgets(buffer, BUFFER_SIZE, stdin);
        write_all(sock, buffer, strlen(buffer)); // Send the line

        // Read the server's response (this IS a line)
        char response_text[BUFFER_SIZE];
        if (read_line(sock, response_text, BUFFER_SIZE) <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        
        printf("Server: %s\n", response_text);

        buffer[strcspn(buffer, "\n")] = 0; // Remove newline for "quit" check
        if (strcmp(buffer, "quit") == 0) {
            break;
        }
    }
    close(sock);
    return 0;
}