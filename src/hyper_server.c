#include "hyper.h"
#include <stdio.h>
#include <errno.h>

int main(void)
{
    int iResult = 0;
    SOCKET sockServer = 0;
    SOCKET sockClient = 0;
        
    iResult = HyperNetworkInit();
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperNetworkInit failed\n");
        return -1;
    }
    else
        printf("[+] Hyper NetAPI Initialized\n");

    iResult = HyperStartServer(&sockServer, 2090);
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperStartServer failed: ERRNO %d\n", errno);
        return -1;
    }
    else
        printf("[+] Hyper Server has been started\n");

    iResult = HyperServerListen(sockServer, &sockClient);
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperServerListen failed\n");
        return -1;
    }
    else
        printf("[+] Connection found\n");

    char *command = malloc(sizeof(*command) * MAX_COMMAND_LENGTH);
    if (command)
    {
        iResult = HyperRecieveCommand(sockClient, command);
        if (iResult != HYPER_SUCCESS)
        {
            printf("[-] HyperRecieveCommand failed\n");
            return -1;
        }
        else
            printf("[+] Command recieved. %s\n", command);

        if (strcmp(command, "SEND") == 0)
        {
            free(command);

            printf("[+] Opening test file...\n");
            
            FILE *file = fopen("test.png", "rb");
            void *fileBuffer = 0;
            size_t fileSize = 0;

            printf("[+] Reading file into buffer...\n");
            iResult = HyperReadFile(file, &fileBuffer, &fileSize);
            if (iResult != HYPER_SUCCESS)
            {
                printf("[-] readall failed\n");
                return -1;
            }
            fclose(file);
            
            printf("[+] Sending file... File Size: %lu\n", fileSize);
            iResult = HyperSendFile(sockClient, fileBuffer, fileSize);
            if (iResult != HYPER_SUCCESS)
            {
                printf("[-] HyperSendFile failed\n");
                return -1;
            }
            else
                printf("[+] File sent successfully!\n");
            
            free(fileBuffer);
        }
    }

    HyperCloseSocket(sockServer);
    HyperCloseSocket(sockClient);
    HyperSocketCleanup();
    return HYPER_SUCCESS;
}
