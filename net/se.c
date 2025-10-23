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
    {"CCC", "ccc"}
};
const int NUM_USERS = sizeof(user_db) / sizeof(struct user);


void *handle_client(void *arg) {
    int client_sock = *(int*)arg;
    free(arg);

    char username[USERNAME_MAX_LEN + 1] = {0};
    char password[PASSWORD_MAX_LEN + 1] = {0};
    char buffer[BUFFER_SIZE];

    // --- 1. Authentication Phase ---
    uint8_t opcode;
    if (read_all(client_sock, &opcode, 1) != 0 || opcode != 'A') {
        fprintf(stderr, "Invalid opcode or connection closed during auth.\n");
        close(client_sock);
        return NULL;
    }

    uint8_t username_len, password_len;
    read_all(client_sock, &username_len, 1);
    read_all(client_sock, username, username_len);
    read_all(client_sock, &password_len, 1);
    read_all(client_sock, password, password_len);

    int authenticated = 0;
    for (int i = 0; i < NUM_USERS; i++) {
        if (strcmp(user_db[i].username, username) == 0 &&
            strcmp(user_db[i].password, password) == 0) {
            authenticated = 1;
            break;
        }
    }

    char response[2] = {'R', authenticated ? '0' : '1'};
    write_all(client_sock, response, 2);
    
    if (!authenticated) {
        printf("Authentication failed for user '%s'. Closing connection.\n", username);
        close(client_sock);
        return NULL;
    }
    printf("User '%s' authenticated successfully.\n", username);

    // --- 2. Message Exchange Phase ---
    while (1) {
        if (read_all(client_sock, &opcode, 1) != 0 || opcode != 'M') {
             fprintf(stderr, "Invalid opcode or connection closed by %s.\n", username);
             break;
        }

        uint16_t msg_len_net;
        read_all(client_sock, &msg_len_net, sizeof(uint16_t));
        uint16_t msg_len = ntohs(msg_len_net);

        if (msg_len >= BUFFER_SIZE) {
            fprintf(stderr, "Message too long from %s.\n", username);
            break;
        }

        read_all(client_sock, buffer, msg_len);
        buffer[msg_len] = '\0';
        printf("Received from '%s': %s\n", username, buffer);
        
        char response_text[BUFFER_SIZE];
        if (strcmp(buffer, "quit") == 0) {
            strcpy(response_text, "Good-bye!");
        } else {
            // This safer approach prevents the compiler warning.
            const char *prefix = "You said ";
            size_t prefix_len = strlen(prefix);

            // Copy the prefix into the response buffer.
            strncpy(response_text, prefix, BUFFER_SIZE - 1);
            response_text[BUFFER_SIZE - 1] = '\0'; // Ensure null termination

            // Append the client's message, but only up to the remaining space.
            // We subtract 1 again for the null terminator.
            strncat(response_text, buffer, BUFFER_SIZE - prefix_len - 1);
        }

        uint16_t response_len = strlen(response_text);
        uint16_t response_len_net = htons(response_len);
        
        char server_response_opcode = 'S';
        write_all(client_sock, &server_response_opcode, 1);
        write_all(client_sock, &response_len_net, sizeof(uint16_t));
        write_all(client_sock, response_text, response_len);
        
        if (strcmp(buffer, "quit") == 0) {
            printf("Client '%s' disconnected.\n", username);
            break;
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

