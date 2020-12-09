#include <hyper/hyper_network.h>

HYPERSTATUS 
HyperNetworkInit(void)
{
    HYPERSTATUS iResult = 0;
#ifdef _WIN32
    WSADATA wsaData = {};
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    return iResult;
}

HYPERSTATUS 
HyperCloseSocket(
    SOCKET              sock) 
{
    HYPERSTATUS iResult = 0;

#ifdef _WIN32
    iResult = closesocket(sock);
#else
    iResult = close(sock);
#endif

    return iResult;
}

HYPERSTATUS 
HyperSocketCleanup(void)
{
    HYPERSTATUS iResult = 0;
#ifdef _WIN32
    iResult = WSACleanup();
#endif
    return iResult;
}

HYPERSTATUS 
HyperSocket(
    SOCKET              *sock)
{
    SOCKET temp = 0;

    // Generate TCP IPv4 socket
    temp = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (temp == INVALID_SOCKET)
        return INVALID_SOCKET;

    // Set Socket
    *sock = temp;

    return HYPER_SUCCESS;
}

HYPERSTATUS 
HyperConnectServer(
    SOCKET              *sock, 
    const char          *cpServerIP, 
    const unsigned short usPort)
{
    SOCKADDR_IN sin = {0};
    SOCKET temp = 0;
    HYPERSTATUS iResult = 0;

    // Generate SOCKET object
    iResult = HyperSocket(&temp);
    if (iResult == INVALID_SOCKET)
        return INVALID_SOCKET;

    // Set IP and Port to connect to
    sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(cpServerIP);
	sin.sin_port = htons(usPort);

    // Connect to server
    iResult = connect(temp, (SOCKADDR*)&sin, sizeof(sin));
    if (iResult == SOCKET_ERROR)
    {
        HyperCloseSocket(temp);
        HyperSocketCleanup();
        return SOCKET_ERROR;
    }

    // Set socket
    *sock = temp;

    return HYPER_SUCCESS;
}

HYPERSTATUS 
HyperStartServer(
    SOCKET              *sock, 
    const unsigned short usPort)
{
    SOCKADDR_IN sin = {0};
    SOCKET temp = 0;
    HYPERSTATUS iResult = 0;

    // Generate Socket Object
    iResult = HyperSocket(&temp);
    if (temp == INVALID_SOCKET)
        return INVALID_SOCKET;

    // Enable SO_REUSEADDR
    // If something goes wrong, we won't have bind issues later
    int enable = 1;
	iResult = setsockopt(temp, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    if (iResult == SOCKET_ERROR)
    {
        HyperCloseSocket(temp);
        HyperSocketCleanup();
        return SOCKET_ERROR;
    }

    // Set Server IP and Port
    // 0.0.0.0 == Bind to both local and public IPs
    sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("0.0.0.0");
	sin.sin_port = htons(usPort);

    // Bind to address and port
    iResult = bind(temp, (SOCKADDR*)&sin, sizeof(sin));
    if (iResult == SOCKET_ERROR)
    {
        HyperCloseSocket(temp);
        HyperSocketCleanup();
        return SOCKET_ERROR;
    }

    // Set parameter output
    *sock = temp;

    return HYPER_SUCCESS;
}

HYPERSTATUS 
HyperServerListen(
    const SOCKET        sockServer, 
    SOCKET              *sockClient)
{
    HYPERSTATUS iResult = 0;
    SOCKET temp = 0;

    iResult = listen(sockServer, 0);
    if (iResult == SOCKET_ERROR)
        return SOCKET_ERROR;

    temp = accept(sockServer, 0, 0);
    if (temp == INVALID_SOCKET)
        return INVALID_SOCKET;

    *sockClient = temp;

    return HYPER_SUCCESS;
}

HYPERSTATUS 
HyperReceiveCommand(
    const SOCKET        sock, 
    char                *cpCommand,
    size_t              stMaxCommandLength
)
{
    HYPERSTATUS iResult = 0;

    // Recieve command
    iResult = recv(sock, cpCommand, stMaxCommandLength, 0);
    if (iResult == SOCKET_ERROR || iResult == CONNECTION_CLOSED)
        return HYPER_FAILED;

    return HYPER_SUCCESS;
}

HYPERSTATUS 
HyperSendCommand(
    const SOCKET        sock, 
    const char          *szCommand)
{
    HYPERSTATUS iResult = 0;

    if (!szCommand)
        return HYPER_BAD_PARAMETER;

    iResult = send(sock, szCommand, strlen(szCommand), 0);
    if (iResult == SOCKET_ERROR || iResult == CONNECTION_CLOSED)
        return SOCKET_ERROR;

    return HYPER_SUCCESS;
}

HYPERSTATUS
HyperSendStatus(
    const SOCKET         sock,
    const unsigned short status)
{
    HYPERSTATUS hsResult = 0;
    char buffer[255];
    memset(buffer, 0, sizeof(buffer));

    if (!sock)
        return HYPER_BAD_PARAMETER;

    snprintf(buffer, sizeof(buffer), "%u", status);
    
    hsResult = send(sock, buffer, sizeof(buffer), 0);
    if (hsResult == SOCKET_ERROR || hsResult == CONNECTION_CLOSED)
        return HYPER_FAILED;

    return HYPER_SUCCESS;
}

HYPERSTATUS
HyperReceiveStatus(
    const SOCKET        sock,
    unsigned short      *status)
{
    HYPERSTATUS hsResult = 0;
    unsigned short temp = 0;
    char buffer[255];
    memset(buffer, 0, sizeof(buffer));

    if (!sock || !status)
        return HYPER_BAD_PARAMETER;

    hsResult = recv(sock, buffer, sizeof(buffer), 0);
    if (hsResult == SOCKET_ERROR || hsResult == CONNECTION_CLOSED)
        return HYPER_FAILED;

    temp = (unsigned short)strtoul(buffer, NULL, 10);

    *status = temp;

    return HYPER_SUCCESS;
}
