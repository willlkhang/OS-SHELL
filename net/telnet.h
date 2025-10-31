#ifndef TELNET_H
#define TELNET_H

#include <unistd.h>
#include <stdint.h>

ssize_t send_all(int sock, const void *buf, size_t len);
ssize_t recv_all(int sock, void *buf, size_t len);

#endif //TELNET_H