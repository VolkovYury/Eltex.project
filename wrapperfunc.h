#ifndef WRAPPERFUNC
#define WRAPPERFUNC

#include <sys/types.h>
#include <sys/socket.h>

int Socket(int domain, int type, int protocol);
void Inet_pton(int af, const char *src, void *dst);
void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Shutdown(int sockfd, int how);
void Close(int fd);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
ssize_t Send(int sockfd, const void *buf, size_t len, int flags);
ssize_t Recv(int sockfd, void *buf, size_t len, int flags);

#endif
