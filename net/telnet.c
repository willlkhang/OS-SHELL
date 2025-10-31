#include "telnet.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

ssize_t send_all(int sock, const void *buf, size_t len) {
    const uint8_t *p = buf;
    size_t remaining = len;
    while (remaining > 0) {
        ssize_t sent = send(sock, p, remaining, 0);
        if (sent <= 0) {
            if (sent < 0 && errno == EINTR) continue;
            return -1;
        }
        p += sent;
        remaining -= sent;
    }
    return (ssize_t)len;
}

ssize_t recv_all(int sock, void *buf, size_t len) {
    uint8_t *p = buf;
    size_t remaining = len;
    while (remaining > 0) {
        ssize_t r = recv(sock, p, remaining, 0);
        if (r <= 0) {
            if (r < 0 && errno == EINTR) continue;
            return -1;
        }
        p += r;
        remaining -= r;
    }
    return (ssize_t)len;
}