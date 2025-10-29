#include <stdio.h>
#include <sys/socket.h>

#include "stream.h"

int read_all(int sockfd, void *buf, size_t len){
    size_t bytes_read = 0;
    while(bytes_read < len) {
        ssize_t res = recv(sockfd, (char*)buf + bytes_read, len - bytes_read, 0);
        if (res <= 0) return -1;
        bytes_read += res;
    }
    return 0;
}

int write_all(int sockfd, const void *buf, size_t len){
    size_t bytes_sent = 0;
    while(bytes_sent < len) {
        ssize_t res = send(sockfd, (const char*)buf + bytes_sent, len - bytes_sent, 0);
        if (res < 0) return -1;
        bytes_sent += res;
    }
    return 0;
}

ssize_t read_line (int sockfd, void *buf, size_t maxlen){
    char *p_buf = (char*)buf;
    size_t bytes_read  = 0;
    unsigned char c; //this unsigned to handle the code 255

    while(bytes_read < maxlen - 1){
        ssize_t res = recv(sockfd, &c, 1, 0);

        if(res <= 0) return res;

        if(c == TELNET_IAC){
            unsigned char command;
            unsigned char option;

            //command will, wont, do, dont
            if(recv(sockfd, &command, 1, 0) <= 0) return -1;
            //optoin echo sga
            if(recv(sockfd, &option, 1, 0) <= 0) return -1;

            continue; //avoding adding this commend to buffer, continue to loop

        }

        if(c == '\n') break; //EOF
        if(c != '\r') p_buf[bytes_read++] = c; //ignore crrage
    }

    p_buf[bytes_read] = '\0'; //null terminate the string
    return bytes_read;
} 

//----telnet methods----
void telnet_send_cmd(int sockfd, unsigned char cmd, unsigned char opt){
    unsigned char buf[3] = {TELNET_IAC, cmd, opt};
    write_all(sockfd, buf, 3);
}

void telnet_write(int sockfd, const char *buf, size_t len) {
    const char *p = buf;
    for (size_t i = 0; i < len; i++) {
        if (*p == (char)TELNET_IAC) {
            // Escape IAC by sending it twice
            write_all(sockfd, TELNET_IAC, 1);
        }
        write_all(sockfd, p, 1);
        p++;
    }
}

void telnet_write_ln(int sockfd, const char *line){
    telnet_write(sockfd, line, strlen(line));
    telnet_write(sockfd, "\r\n", 2);
}

ssize_t telnet_read_char(int sockfd, char *c) {
    unsigned char raw_c;
    ssize_t res;

    while (1) {
        res = recv(sockfd, &raw_c, 1, 0);
        if (res <= 0) return res; // Error or EOF

        if (raw_c == TELNET_IAC) {
            unsigned char cmd, opt;
            if (recv(sockfd, &cmd, 1, 0) <= 0) return -1;
            if (cmd < TELNET_SB) {
                // 2-byte command
                fprintf(stderr, "[DEBUG] Recv IAC %d\n", cmd);
                continue; // Ignore and keep reading
            }
            if (recv(sockfd, &opt, 1, 0) <= 0) return -1;
            
            fprintf(stderr, "[DEBUG] Recv IAC %d %d\n", cmd, opt);

            switch (cmd) {
                case TELNET_WILL:
                    if (opt == TELOPT_ECHO || opt == TELOPT_SGA) {
                        telnet_send_cmd(sockfd, TELNET_DO, opt);
                    } else {
                        telnet_send_cmd(sockfd, TELNET_DONT, opt);
                    }
                    break;
                
                case TELNET_DO:
                    if (opt == TELOPT_SGA) {
                        telnet_send_cmd(sockfd, TELNET_WILL, opt);
                    } else {
                        // We WONT do anything else (like ECHO)
                        telnet_send_cmd(sockfd, TELNET_WONT, opt);
                    }
                    break;

                case TELNET_WONT: // Peer refuses
                case TELNET_DONT: // Peer refuses
                    // We just accept their refusal
                    break;
            }
            // This was a command, not data, so loop again
            continue;
        }

        // --- Handle \r ---
        if (raw_c == '\r') {
            // Telnet mandates \r is followed by \n or \0.
            // We just strip it and wait for the \n.
            continue;
        }

        *c = (char)raw_c;
        return 1;
    }
}

ssize_t telnet_read_line(int sockfd, char *buf, size_t maxlen) {
    char *p_buf = (char*)buf;
    size_t bytes_read = 0;
    char c;
    ssize_t res;

    while (bytes_read < maxlen - 1) {
        res = telnet_read_char(sockfd, &c);
        if (res <= 0) return res; // Error or EOF

        p_buf[bytes_read++] = c;

        if (c == '\n') {
            break; // End of line
        }
    }

    p_buf[bytes_read] = '\0'; // Null-terminate
    // Remove the newline from the buffer for consistency
    if (bytes_read > 0 && p_buf[bytes_read - 1] == '\n') {
        p_buf[bytes_read - 1] = '\0';
        bytes_read--;
    }
    return bytes_read;
}

ssize_t telnet_read_prompt(int sockfd, char *buf, size_t len) {
    char *p_buf = (char*)buf;
    size_t bytes_read = 0;
    char c;
    ssize_t res;

    while (bytes_read < len) {
        res = telnet_read_char(sockfd, &c);
        if (res <= 0) return res; // Error or EOF
        p_buf[bytes_read++] = c;
    }
    
    p_buf[bytes_read] = '\0'; // Null-terminate
    return bytes_read;
}
