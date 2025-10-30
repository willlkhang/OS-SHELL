#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#include "stream.h"

#define PORT 65432
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define USERNAME_MAX_LEN 255
#define PASSWORD_MAX_LEN 255

// --- User "Database" ---
struct user {
    char username[USERNAME_MAX_LEN + 1];
    char password[PASSWORD_MAX_LEN + 1];
};

struct user user_db[] = {
    {"AAA", "aaa"},
    {"BBB", "bbb"},
    {"CCC", "ccc"},
    {"test", "test"}
};
const int NUM_USERS = sizeof(user_db) / sizeof(user_db[0]);


void *handle_client(void *arg) {
    int client_sock = *(int*)arg;
    free(arg);

    char username[USERNAME_MAX_LEN + 1] = {0};
    char password[PASSWORD_MAX_LEN + 1] = {0};
    char buffer[BUFFER_SIZE];

    ssize_t read_status;

    // write_all(client_sock, "Username: ", 10);
    // read_status = read_line(client_sock, username, USERNAME_MAX_LEN + 1);
    // if(read_status <= 0){
    //     fprintf(stderr, "Connection terminated during username prompt\n");
    //     close(client_sock);
    //     return NULL;
    // }

    // write_all(client_sock, "Password: ", 10);
    // read_status = read_line(client_sock, password, PASSWORD_MAX_LEN + 1);
    // if(read_status <= 0){
    //     fprintf(stderr, "Connection terminated during password prompt\n");
    //     close(client_sock);
    //     return NULL;
    // }

    // --- 1. TELNET NEGOTIATION ---
    // Server: "I WILL Echo characters."
    telnet_send_cmd(client_sock, TELNET_WILL, TELOPT_ECHO);
    // Server: "I WILL Suppress Go Ahead."
    telnet_send_cmd(client_sock, TELNET_WILL, TELOPT_SGA);
    // Client will respond, and telnet_read_char will handle it.

    // --- 2. AUTHENTICATION ---
    // Use telnet_write (no \r\n), not writeln
    telnet_write(client_sock, "Username: ", 10);
    // Use telnet_read_line (handles commands)
    read_status = telnet_read_line(client_sock, username, USERNAME_MAX_LEN + 1);
    if(read_status < 0){ // Note: 0 is a valid empty line
        fprintf(stderr, "Connection terminated during username prompt\n");
        close(client_sock);
        return NULL;
    }

    telnet_write(client_sock, "Password: ", 10);
    read_status = telnet_read_line(client_sock, password, PASSWORD_MAX_LEN + 1);
    if(read_status < 0){
        fprintf(stderr, "Connection terminated during password prompt\n");
        close(client_sock);
        return NULL;
    }


    int authenticated = 0;
    for(int i = 0; i < NUM_USERS; i++){ //check if the enter account is on database
        if(strcmp(user_db[i].username, username) == 0 && strcmp(user_db[i].password, password) == 0){
            authenticated = 1;
            break;
        }
    }

    if(!authenticated){ //throw error exception for incorrect account
        printf("Username or Password is incorrect, Please check again...\n");
        //telnet_writeln (adds \r\n)
        telnet_writeln(client_sock, "Authentication failed.");
        close(client_sock);
        return NULL;
    }

    //otherwise, when authentication is 1 which is found from the db
    printf("User '%s' authenticated successfully\n", username);
    telnet_writeln(client_sock, "Authentication successful!");

    while(1){
        telnet_write(client_sock, "> ", 2); 

        read_status = telnet_read_line(client_sock, buffer, BUFFER_SIZE);
        if(read_status <= 0){
            fprintf(stderr, "Connection closed by %s\n", username);
            break;
        }

        printf("Received from %s: %s\n", username, buffer);

        if(strcmp(buffer, "quit") == 0 || strcmp(buffer, "bye") == 0){
            telnet_writeln(client_sock, "Goodbye!");
            printf("Client %s disconnected\n", username);
            break;
        }
        else {
            char response_text[BUFFER_SIZE + 25];
            snprintf(response_text, sizeof(response_text), "You said: %s", buffer);
            telnet_writeln(client_sock, response_text);
        }
    }
    close(client_sock);
    return NULL;
}


int main() {
    int server_sock;
    struct sockaddr_in server_addr;
    
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d\n", PORT);
    
    while(1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        
        if (client_sock < 0) {
            perror("accept");
            continue;
        }
        
        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        pthread_t tid;
        int *p_client_sock = malloc(sizeof(int));
        *p_client_sock = client_sock;
        
        if (pthread_create(&tid, NULL, handle_client, p_client_sock) != 0) {
            perror("pthread_create");
            free(p_client_sock);
        }
        pthread_detach(tid);
    }
    
    close(server_sock);
    return 0;
}

