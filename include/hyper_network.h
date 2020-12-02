#ifndef _NETWORK_H
#define _NETWORK_H

#ifdef _WIN32
    #define _WINSOCK_DEPRECATED_NO_WARNINGS // Make WinSock STFU
    #include <winsock2.h> 
    #pragma comment (lib, "Ws2_32.lib") // Link to WinSock
    
    typedef int SOCKLEN;
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
    
    // WinSock2 uses unsigned int for sockets, while POSIX uses int
    typedef int SOCKET;
    typedef struct sockaddr_in SOCKADDR_IN;
    typedef struct sockaddr SOCKADDR;
    typedef socklen_t SOCKLEN;

    #define INVALID_SOCKET  -1
    #define SOCKET_ERROR    -1
#endif

#include "hyper.h"
#include <string.h>

int     HyperNetworkInit(void);
int     HyperCloseSocket(SOCKET sock);
int     HyperSocketCleanup(void);
int     HyperSocket(SOCKET *sock);
int     HyperConnectServer(SOCKET *sock, const char *cpServerIP, unsigned short usPort);
int     HyperStartServer(SOCKET *sock, unsigned short usPort);
int     HyperServerListen(SOCKET sockServer, SOCKET *sockClient);
int     HyperRecieveCommand(SOCKET sock, char *cpCommand);
int     HyperSendCommand(SOCKET sock, const char *cpCommand);

#endif
