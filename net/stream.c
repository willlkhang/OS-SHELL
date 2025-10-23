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