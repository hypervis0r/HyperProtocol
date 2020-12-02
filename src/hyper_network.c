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

    return HYPER_SUCCESS;
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

    return HYPER_SUCCESS;
}

int HyperStartServer(SOCKET *sock, unsigned short usPort)
{
    SOCKADDR_IN sin = {};
    int iResult = 0;

    iResult = HyperSocket(sock);
    if (*sock == INVALID_SOCKET)
        return INVALID_SOCKET;
    printf("[+] Aquirred Socket\n");

    int enable = 1;
	iResult = setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    if (iResult == SOCKET_ERROR)
    {
        HyperCloseSocket(*sock);
        HyperSocketCleanup();
        return SOCKET_ERROR;
    }
    printf("[+] Set SO_REUSEADDR\n");

    sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("0.0.0.0"); // 0.0.0.0 == Local and Public IP
	sin.sin_port = htons(usPort);

    iResult = bind(*sock, (SOCKADDR*)&sin, sizeof(sin));
    if (iResult == SOCKET_ERROR)
    {
        HyperCloseSocket(*sock);
        HyperSocketCleanup();
        return SOCKET_ERROR;
    }
    printf("[+] Bound to IP/Port\n");

    return HYPER_SUCCESS;
}

int HyperServerListen(SOCKET sockServer, SOCKET *sockClient)
{
    int iResult = 0;

    iResult = listen(sockServer, 0);
    if (iResult == SOCKET_ERROR)
        return SOCKET_ERROR;

    *sockClient = accept(sockServer, 0, 0);
    if (*sockClient == INVALID_SOCKET)
        return INVALID_SOCKET;

    return HYPER_SUCCESS;
}

int HyperRecieveCommand(SOCKET sock, char *cpCommand)
{
    int iResult = 0;

    if (!cpCommand)
        return BAD_PARAMETER;

    iResult = recv(sock, cpCommand, MAX_COMMAND_LENGTH, 0);
    if (iResult == SOCKET_ERROR)
        return SOCKET_ERROR;

    return HYPER_SUCCESS;
}

int HyperSendCommand(SOCKET sock, const char *szCommand)
{
    int iResult = 0;

    if (!szCommand)
        return BAD_PARAMETER;

    iResult = send(sock, szCommand, strlen(szCommand), 0);
    if (iResult == SOCKET_ERROR)
        return SOCKET_ERROR;

    return HYPER_SUCCESS;
}
