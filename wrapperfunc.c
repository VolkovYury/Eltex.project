#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "wrapperfunc.h"

int Socket(int domain, int type, int protocol)
{
        int result = socket(domain, type, protocol);

        if (result == -1) {
                perror("Warning: socket function failed\n");
                exit(EXIT_FAILURE);
        }

        return result;
}

void Inet_pton(int af, const char *src, void *dst)
{
        int result = inet_pton(af, src, dst);

        if (result == 0) {
                perror("Warning: inet_pton function failed: *src does not contain a character string representing a valid "
                       "network address in the specified address family\n");
                exit(EXIT_FAILURE);
        } else if (result == -1) {
                perror("Warning: inet_pton function failed\n");
                exit(EXIT_FAILURE);
        }
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
        int result = connect(sockfd, addr, addrlen);

        if (result == 0) {
                printf("Server connection successful!\n\n\n");

        } else if (result == -1) {
                perror("Warning: connect function failed\n");
                exit(EXIT_FAILURE);
        }
}

void Shutdown(int sockfd, int how)
{
        int result = shutdown(sockfd, how);

        if (result == -1) {
                perror("Warning: shutdown function failed\n");
                exit(EXIT_FAILURE);
        }
}

void Close(int fd)
{
        int result = close(fd);

        if (result == -1) {
                perror("Warning: close function failed\n");
                exit(EXIT_FAILURE);
        }
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
        int result = bind(sockfd, addr, addrlen);

        if (result == -1) {
                perror("Warning: bind function failed\n");
                exit(EXIT_FAILURE);
        }
}

void Listen(int sockfd, int backlog)
{
        int result = listen(sockfd, backlog);

        if (result == -1) {
                perror("Warning: listen function failed\n");
                exit(EXIT_FAILURE);
        }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
        int result = accept(sockfd, addr, addrlen);

        if (result == -1) {
                perror("Warning: accept function failed\n");
                exit(EXIT_FAILURE);
        }

        return result;
}

ssize_t Send(int sockfd, const void *buf, size_t len, int flags)
{
      ssize_t result = send(sockfd, buf, len, flags);

      if (result == -1) {
              perror("Warning: send function failed\n");
              exit(EXIT_FAILURE);
      }

      return result;
}

ssize_t Recv(int sockfd, void *buf, size_t len, int flags)
{
        ssize_t result = recv(sockfd, buf, len, flags);

        if (result == -1) {
                perror("Warning: recv function failed\n");
                exit(EXIT_FAILURE);
        }

        return result;
}



