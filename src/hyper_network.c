#include "hyper_network.h"

int HyperNetworkInit(void)
{
    int iResult = 0;
#ifdef _WIN32
    WSADATA wsaData = {};
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    return iResult;
}

int HyperCloseSocket(SOCKET sock)
{
    int iResult = 0;

#ifdef _WIN32
    iResult = closesocket(sock);
#else
    iResult = close(sock);
#endif

    return iResult;
}

int HyperSocketCleanup(void)
{
    int iResult = 0;
#ifdef _WIN32
    iResult = WSACleanup();
#endif
    return iResult;
}

int HyperSocket(SOCKET *sock)
{
    *sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (*sock == INVALID_SOCKET)
    {
        HyperCloseSocket(*sock);
        HyperSocketCleanup();
        return INVALID_SOCKET;
    }

    return 0;
}

int HyperConnectServer(SOCKET *sock, const char *cpServerIP, unsigned short usPort)
{
    SOCKADDR_IN sin = {};
    int iResult = 0;

    iResult = HyperSocket(sock);
    if (iResult == INVALID_SOCKET)
        return INVALID_SOCKET;

    sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(cpServerIP);
	sin.sin_port = htons(usPort);

    iResult = connect(*sock, (SOCKADDR*)&sin, sizeof(sin));
    if (iResult == SOCKET_ERROR)
    {
        HyperCloseSocket(*sock);
        HyperSocketCleanup();
        return SOCKET_ERROR;
    }

    return 0;
}

int HyperStartServer(SOCKET *sock, unsigned short usPort)
{
    SOCKADDR_IN sin = {};
    int iResult = 0;

    iResult = HyperSocket(sock);
    if (*sock == INVALID_SOCKET)
        return INVALID_SOCKET;

    sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0; // 0.0.0.0 == Local and Public IP
	sin.sin_port = htons(usPort);

    int enable = 1;
	iResult = setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    if (iResult == SOCKET_ERROR)
    {
        HyperCloseSocket(*sock);
        HyperSocketCleanup();
        return SOCKET_ERROR;
    }

    iResult = bind(*sock, (SOCKADDR*)&sin, sizeof(sin));
    if (iResult == SOCKET_ERROR)
    {
        HyperCloseSocket(*sock);
        HyperSocketCleanup();
        return SOCKET_ERROR;
    }

    return 0;
}

int HyperServerListen(SOCKET sockServer, SOCKET *sockClient, SOCKADDR_IN *sin)
{
    int iResult = 0;
    SOCKLEN slSinLen = 0;

    iResult = listen(sockServer, 0);
    if (iResult == SOCKET_ERROR)
        return SOCKET_ERROR;

    slSinLen = sizeof(*sin);
    *sockClient = accept(sockServer, (SOCKADDR*)&sin, &slSinLen);
    if (*sockClient == INVALID_SOCKET)
        return INVALID_SOCKET;

    return 0;
}
