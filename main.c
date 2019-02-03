#include "myHelper.h"
#include "receiver.h"
#include "sender.h"
#include <getopt.h>

#define FSENDER 1
#define FRECEIVER 2
#define FADDRESS 4
#define FPORT 8
#define FSAVETO 16
#define FFILE 32


const int senderFlag = FSENDER|FADDRESS|FPORT|FFILE;
const int receiverFlag = FRECEIVER|FPORT|FSAVETO;

struct globalArgs_t
{
    // int role;
    int flag;
    const char *filepath;
    const char *saveTo;
    const char *host;
    short port;
} globalArgs;

static const char *optString = "sra:p:t:f:h?";

void usage(char *programName)
{
    printf("usage: %s [ -s -a host -p port -f file | -r -p port -t saveto]  \n", programName);
    _exit(-1);
}


void getOpt(int argc, char **argv)
{
    int opt;
    globalArgs.flag = 0;
    globalArgs.filepath = NULL;
    globalArgs.host = NULL;
    globalArgs.port = 0;
    globalArgs.saveTo = NULL;

    while ((opt = getopt(argc, argv, optString)) != -1)
    {
        switch (opt)
        {
        case 's':
            globalArgs.flag = globalArgs.flag | FSENDER;
            break;
        case 'r':
            globalArgs.flag = globalArgs.flag | FRECEIVER;
            break;
        case 'a':
            globalArgs.flag = globalArgs.flag | FADDRESS;
            globalArgs.host = optarg;
            break;
        case 'p':
            globalArgs.flag = globalArgs.flag | FPORT;
            globalArgs.port = atoi(optarg);
            break;
        case 't':
            globalArgs.flag = globalArgs.flag | FSAVETO;
            globalArgs.saveTo = optarg;
            break;
        case 'f':
              globalArgs.flag = globalArgs.flag | FFILE;
            globalArgs.filepath = optarg;
            break;
        case '?':
        case 'h':
            usage(argv[0]);
            break;
        default:
            usage(argv[0]);
            break;
        }
    }
    return;
}

void _send()
{
    struct sender *sender = initSender();
    connectTo(sender, globalArgs.host, globalArgs.port);
    sendFile(sender, globalArgs.filepath);
    destroySender(sender);
}

void _receive()
{
    struct receiver *receiver = initReceiver(globalArgs.port);
    wait4connect(receiver);
    receiveFile(receiver, globalArgs.saveTo);
    destroy(receiver);
}

int main(int argc, char **argv)
{
    getOpt(argc, argv);
    if(globalArgs.flag == senderFlag)
    {
        _send();
    }
    else if(globalArgs.flag == receiverFlag)
    {
        _receive();
    }
    else
    {
        usage(argv[0]);
    }
    return 0;
}