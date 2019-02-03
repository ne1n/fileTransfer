#ifndef MYHELPER_H
#define MYHELPER_H
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdarg.h>

void errExit(const char *format, ...)
{
    char buffer[100];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    char error[100] = "[ERROR EXIT]: ";
    strcat(error, buffer);
    perror(error);
    _exit(-1);
}
#endif