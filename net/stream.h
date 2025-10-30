#include <stdio.h>
#include <sys/socket.h>

//telnet commands
#define TELNET_IAC 255
#define TELNET_DONT 254
#define TELNET_DO 253
#define TELNET_WONT 252
#define TELNET_WILL 251
#define TELNET_SB 250
#define TELNET_GA 249
#define TELNET_EL 248
#define TELNET_EC 247
#define TELNET_AYT 246
#define TELNET_AO 245
#define TELNET_IP 244
#define TELNET_BRK 243
#define TELNET_DM 242
#define TELNET_NOP 241
#define TELNET_SE 240

//telnet option
#define TELOPT_ECHO 1 // echo
#define TELOPT_SGA 3 // suppress go ahead


int read_all(int sockfd, void *buf, size_t len);
int write_all(int sockfd, const void *buf, size_t len);

ssize_t read_line (int sockfd, void *buf, size_t maxlen);

/**
 * @brief Sends a 3-byte Telnet command (e.g., IAC WILL ECHO).
 */
void telnet_send_cmd(int sockfd, unsigned char cmd, unsigned char opt);
/**
 * @brief Writes data to the socket, properly escaping any IAC bytes.
 */
void telnet_write(int sockfd, const char *buf, size_t len);
/**
 * @brief Writes a line of text, escaping IACs and appending \r\n.
 */
void telnet_writeln(int sockfd, const char *line);

/**
 * @brief Reads one "clean" data character.
 * * This is the core of the client. It reads from the socket,
 * handles any incoming Telnet negotiations transparently,
 * and returns a single data byte.
 * * @param sockfd The socket.
 * @param c Pointer to store the resulting character.
 * @return 1 on success, 0 on EOF, -1 on error.
 */
ssize_t telnet_read_char(int sockfd, char *c);
/**
 * @brief Reads a line of text (ending in \n).
 * Uses telnet_read_char() to build the line.
 * The \n is included, \r is stripped.
 */
ssize_t telnet_read_line(int sockfd, char *buf, size_t maxlen);
/**
 * @brief Reads a fixed-size prompt.
 * Uses telnet_read_char() to read exactly 'len' bytes.
 */
ssize_t telnet_read_prompt(int sockfd, char *buf, size_t len);