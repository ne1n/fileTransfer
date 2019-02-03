#ifndef SENDER_H
#define SENDER_H
#include "myHelper.h"
#define BACKLOG 5
struct sender
{
    int sfd;
    struct sockaddr_in s_addr;
};

struct sender *initSender()
{
    struct sender *sender = (struct sender *)malloc(sizeof(struct sender));
    sender->sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sender->sfd == -1)
        errExit("when create socket");
    return sender;
}

void connectTo(struct sender *sender, const char *host, short port)
{
    memset(&sender->s_addr, 0, sizeof(struct sender));
    sender->s_addr.sin_family = AF_INET;
    sender->s_addr.sin_addr.s_addr = inet_addr(host);
    sender->s_addr.sin_port = htons(port);
    printf("connecting to %s:%hu\n", inet_ntoa(sender->s_addr.sin_addr), ntohs(sender->s_addr.sin_port));
    if (connect(sender->sfd, (struct sockaddr *)&sender->s_addr, sizeof(struct sockaddr)))
        errExit("when connect to %s:%hu", inet_ntoa(sender->s_addr.sin_addr), ntohs(sender->s_addr.sin_port));
}

void sendFile(struct sender *sender, const char *filepath)
{
    off_t totalbytes = 0;
    int ifd;
    char buffer[BUFSIZ];
    ifd = open(filepath, O_RDONLY);
    if (ifd == -1)
        errExit("when open file %s", filepath);
    printf("sending file %s ...\n", filepath);
    char *filename = basename(filepath);
    write(sender->sfd, filename, FILENAME_MAX);
    struct stat buf;
    fstat(ifd, &buf);
    char *fileSZ = (char *)malloc(sizeof(__off_t));
    snprintf(fileSZ, sizeof(__off_t), "%lu", buf.st_size);
    write(sender->sfd, fileSZ, sizeof(__off_t));

    while (totalbytes != buf.st_size)
    {
        sendfile(sender->sfd, ifd, &totalbytes, BUFSIZ);
        printf("progress: %luKB/%luKB", totalbytes/1024, buf.st_size/1024);
        printf("\r");
        fflush(stdout);
    }
    printf("\n");
    printf("file sended!\n");
}

void destroySender(struct sender *sender)
{
    close(sender->sfd);
    free(sender);
}
#endif