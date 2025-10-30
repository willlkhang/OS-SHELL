#include <stdio.h>
#include <sys/socket.h>
#include <string.h>

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
// 3 bytes command: IAC <cmd> <opt>
void telnet_send_cmd(int sockfd, unsigned char cmd, unsigned char opt){
    unsigned char buf[3] = {TELNET_IAC, cmd, opt};
    write_all(sockfd, buf, 3);
}

void telnet_write(int sockfd, const char *buf, size_t len) {
    for(size_t i = 0; i < len; i++){
        unsigned char c = (unsigned char)buf[i];
        if(c == TELNET_IAC)
            write_all(sockfd, "\xff", 1);
        write_all(sockfd, &c, 1);
    }
}

void telnet_writeln(int sockfd, const char *line){
    telnet_write(sockfd, line, strlen(line));
    telnet_write(sockfd, "\r\n", 2);
}

ssize_t telnet_read_char(int sockfd, char *c) {
    unsigned char ch;
    while (1) {
        ssize_t r = recv(sockfd, &ch, 1, 0);
        if (r <= 0) return r;               /* error or EOF */

        if (ch == TELNET_IAC) {
            unsigned char cmd, opt = 0;
            if (recv(sockfd, &cmd, 1, 0) <= 0) return -1;

            /* 3-byte commands (WILL/WONT/DO/DONT) */
            if (cmd >= TELNET_WILL && cmd <= TELNET_DONT) {
                if (recv(sockfd, &opt, 1, 0) <= 0) return -1;
            }

            /* ---------- NEGOTIATION ---------- */
            switch (cmd) {
                case TELNET_WILL:
                    /* Client offers to do something */
                    if (opt == TELOPT_ECHO) {
                        /* We want to echo, so tell client NOT to */
                        telnet_send_cmd(sockfd, TELNET_DONT, TELOPT_ECHO);
                    } else if (opt == TELOPT_SGA) {
                        telnet_send_cmd(sockfd, TELNET_DO, TELOPT_SGA);
                    } else {
                        telnet_send_cmd(sockfd, TELNET_DONT, opt);
                    }
                    break;

                case TELNET_DO:
                    /* Client asks us to do something */
                    if (opt == TELOPT_ECHO) {
                        /* We WILL echo */
                        telnet_send_cmd(sockfd, TELNET_WILL, TELOPT_ECHO);
                    } else if (opt == TELOPT_SGA) {
                        telnet_send_cmd(sockfd, TELNET_WILL, TELOPT_SGA);
                    } else {
                        telnet_send_cmd(sockfd, TELNET_WONT, opt);
                    }
                    break;

                case TELNET_WONT:
                case TELNET_DONT:
                    /* Just ignore – we already sent our preference */
                    break;

                default:
                    /* Sub-negotiation etc. – ignore for this demo */
                    break;
            }
            continue;            /* command consumed, loop for data */
        }

        /* ----- CR handling (RFC 854) ----- */
        if (ch == '\r') {
            /* Expect NUL or LF next – we simply drop the CR */
            continue;
        }

        *c = (char)ch;
        return 1;
    }
}

ssize_t telnet_read_line(int sockfd, char *buf, size_t maxlen){
    char *p = buf;
    size_t n = 0;
    char ch;
    ssize_t r;

    while (n < maxlen - 1) {
        r = telnet_read_char(sockfd, &ch);
        if (r <= 0) return r;            /* error / EOF */

        if (ch == '\n') break;           /* line end */

        p[n++] = ch;
    }
    p[n] = '\0';
    return (ssize_t)n;
}

ssize_t telnet_read_prompt(int sockfd, char *buf, size_t len){
    char *p = buf;
    size_t n = 0;
    char ch;
    ssize_t r;

    while (n < len) {
        r = telnet_read_char(sockfd, &ch);
        if (r <= 0) return r;
        p[n++] = ch;
    }
    p[n] = '\0';
    return (ssize_t)n;
}
