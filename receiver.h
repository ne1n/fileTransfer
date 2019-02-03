#ifndef RECEIVER_H
#define RECEIVER_H
#include "myHelper.h"
#define BACKLOG 5

struct receiver
{
    int sfd;
    int cfd;
    struct sockaddr_in s_addr;
    struct sockaddr_in c_addr;
};

struct receiver *initReceiver(short port)
{
    struct receiver *receiver = (struct receiver *)malloc(sizeof(struct receiver));
    receiver->sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (receiver->sfd == -1)
        errExit("when create server socket");
    receiver->cfd = -1;
    memset(&receiver->s_addr, 0, sizeof(struct sockaddr_in));
    receiver->s_addr.sin_family = AF_INET;
    receiver->s_addr.sin_addr.s_addr = INADDR_ANY;
    receiver->s_addr.sin_port = htons(port);
    printf("server socket created!\n");
    return receiver;
}

void wait4connect(struct receiver *receiver)
{
    if (bind(receiver->sfd, (struct sockaddr *)&receiver->s_addr, sizeof(struct sockaddr)) == -1)
        errExit("when bind to %s:%hu", inet_ntoa(receiver->s_addr.sin_addr), ntohs(receiver->s_addr.sin_port));
    if (listen(receiver->sfd, BACKLOG) == -1)
        errExit("when listen %s:%hu", inet_ntoa(receiver->s_addr.sin_addr), ntohs(receiver->s_addr.sin_port));
    printf("listening on port %hu ...\n", ntohs(receiver->s_addr.sin_port));
    printf("Waiting for connect ...\n");
    socklen_t len;
    receiver->cfd = accept(receiver->sfd, (struct sockaddr *)&receiver->c_addr, &len);
    if (receiver->cfd == -1)
        errExit("when accept connect");
    printf("accept connect from %s:%hu\n", inet_ntoa(receiver->c_addr.sin_addr), ntohs(receiver->c_addr.sin_port));
}

void receiveFile(struct receiver *receiver, const char *path)
{

    printf("waiting for clent send file ...\n");
    char buffer[BUFSIZ];
    int outFd;
    char saveTo[FILENAME_MAX];
    strncpy(saveTo, path, FILENAME_MAX);
    read(receiver->cfd, buffer, FILENAME_MAX);
    strcat(saveTo, buffer);
    outFd = open(saveTo, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    printf("saving files to %s\n", saveTo);
    if (outFd == -1)
        errExit("when save files to %s", saveTo);
    read(receiver->cfd, buffer, sizeof(__off_t));
    unsigned long fileSZ = strtoul(buffer, NULL, 0);
    printf("receiving ...\n");
    int numRead;
    size_t totalBytes = 0;
    while ((numRead = read(receiver->cfd, buffer, BUFSIZ)) > 0)
    {
        totalBytes += write(outFd, buffer, numRead);
        printf("progress: %luKB/%luKB", totalBytes/1024, fileSZ/1024);
        printf("\r");
        fflush(stdout);
    }
    printf("\n");
    if (numRead == -1)
        errExit("when reading bytes from client");
    close(outFd);
    printf("files received!\n");
}

void destroy(struct receiver *receiver)
{
    close(receiver->sfd);
    close(receiver->cfd);
    free(receiver);
}
#endif