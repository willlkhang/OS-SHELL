#include <stdio.h>

int read_all(int sockfd, void *buf, size_t len);
int write_all(int sockfd, const void *buf, size_t len);

ssize_t read_line (int sockfd, void *buf, size_t maxlen);