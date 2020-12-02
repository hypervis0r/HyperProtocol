#include "hyper.h"
#include <stdio.h>

int main(void)
{
    int iResult = 0;
    SOCKET sockServer = 0;
        
    iResult = HyperNetworkInit();
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperNetworkInit failed\n");
        return -1;
    }
    else
        printf("[+] Hyper NetAPI Initialized\n");

    iResult = HyperConnectServer(&sockServer, "127.0.0.1", 2090);
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperConnectServer failed\n");
        return -1;
    }
    else
        printf("[+] Connected to server\n");

    iResult = HyperSendCommand(sockServer, "SEND");
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperSendCommand failed\n");
        return -1;
    }
    else
        printf("[+] Command sent\n");

    void *lpBuffer = NULL;
    unsigned long ulTotalSize = 0;
    printf("[+] Recieving file...");
    iResult = HyperRecieveFile(sockServer, &lpBuffer, &ulTotalSize);
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperRecieveFile failed\n");
        free(lpBuffer);
        return -1;
    }
    else
        printf("[+] File recieved\n");

    FILE *pFile = fopen("testresult.png", "wb");
    if (pFile)
    {
        fwrite(lpBuffer, ulTotalSize, 1, pFile);
        fclose(pFile);
    }
    free(lpBuffer);

    printf("[+] File written\n");

    HyperCloseSocket(sockServer);
    HyperSocketCleanup();
    return HYPER_SUCCESS;
}
