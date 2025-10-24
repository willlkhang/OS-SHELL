#include <stdio.h>
#include <sys/socket.h>

int read_all(int sockfd, void *buf, size_t len) {
    size_t bytes_read = 0;
    while (bytes_read < len) {
        ssize_t res = recv(sockfd, (char*)buf + bytes_read, len - bytes_read, 0);
        if (res <= 0) return -1;
        bytes_read += res;
    }
    return 0;
}

int write_all(int sockfd, const void *buf, size_t len) {
    size_t bytes_sent = 0;
    while (bytes_sent < len) {
        ssize_t res = send(sockfd, (const char*)buf + bytes_sent, len - bytes_sent, 0);
        if (res < 0) return -1;
        bytes_sent += res;
    }
    return 0;
}

ssize_t read_line (int sockfd, void *buf, size_t maxlen){
    char *p_buf = (char*)buf;
    size_t bytes_read  = 0;
    char c;

    while(bytes_read < maxlen - 1){
        ssize_t res = recv(sockfd, &c, 1, 0);

        if(res <= 0) return res;
        if(c == '\n') break; //EOF
        if(c != '\r') p_buf[bytes_read++] = c; //ignore crrage
    }

    p_buf[bytes_read] = '\0'; //null terminate the string
    return bytes_read;
}