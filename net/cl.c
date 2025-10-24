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
    
    read_line(sock, server_prompt, BUFFER_SIZE); //read the username
    printf("%s", server_prompt); //print prompt

    fgets(username, sizeof(username), stdin); //get password from user input for socket to read
    write_all(sock, username, strlen(username)); //write to socket where server is listening

    readline(sock, server_prompt, stdin); //sever wait to read password
    printf("%s", server_prompt); //print prompt

    fgets(password, sizeof(password), stdin); //get password from user input for socket to read
    write_all(sock, password, strlen(password)); //send to server

    char auth_response[BUFFER_SIZE];
    read_line(sock, auth_response, BUFFER_SIZE);
    printf("Server: %s\n", auth_response);

    if(strcmp(auth_response, "Authentication failed", 21) == 0){
        close(sock); return 0; //exit the program
    }

    //communication 
    while(1){
        //sever open listenning
        readline(sock, server_prompt, BUFFER_SIZE);
        printf("%s", server_prompt);

        //client respond and send to where sever is listening.
        fgets(buffer, BUFFER_SIZE, stdin); //user input from terminal
        write_all(sock, buffer, strlen(buffer)); // send to server

        //read sever respond
        char response_text[BUFFER_SIZE];
        if(read_line(sock, response_text, BUFFER_SIZE) <= 0){
            printf("Sever disconnected\n"); break;
        }
        printf("Server: %s\n", response_text); // display to user what sever respond

        buffer[strcspn(buffer, '\n')] == 0; //skipp enter key
        if(strcmp(buffer, "quit") == 0 || strcmp(buffer, "bye") == 0) break;
    }
    close(sock);
    return 0;
}