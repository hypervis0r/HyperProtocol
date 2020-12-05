#include "hyper.h"
#include <stdio.h>
#include <errno.h>

void usage(void)
{
    puts( 
        "    __  __                     _____                               \n"
        "   / / / /_  ______  ___  ____/ ___/___  ______   _____  _____     \n"
        "  / /_/ / / / / __ \\/ _ \\/ ___|__ \\/ _ \\/ ___/ | / / _ \\/ ___/\n"
        " / __  / /_/ / /_/ /  __/ /  ___/ /  __/ /   | |/ /  __/ /         \n"
        "/_/ /_/\\__  / ____/\\___/_/  /____/\\___/_/    |___/\\___/_/      \n"
        "      /____/_/                                                     \n"
    );
    puts("Usage: hyper-server <PORT>");
}


int main(int argc, char **argv)
{
    HYPERSTATUS iResult = 0;
    SOCKET sockServer = 0;
    SOCKET sockClient = 0;
    unsigned short usPort = 0;

    if (argc < 2)
    {
        usage();
        return HYPER_FAILED;
    }
    else
    {
        usPort = (unsigned short)strtoul(argv[1], NULL, 0);
    }
        
    iResult = HyperNetworkInit();
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperNetworkInit failed\n");
        return HYPER_FAILED;
    }
    else
        printf("[+] Hyper NetAPI Initialized\n");

    iResult = HyperStartServer(&sockServer, usPort);
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperStartServer failed\n");
        return HYPER_FAILED;
    }
    else
        printf("[+] Hyper Server has been started\n");

    iResult = HyperServerListen(sockServer, &sockClient);
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperServerListen failed\n");
        return HYPER_FAILED;
    }
    else
        printf("[+] Connection found\n");

    char *command = NULL;
    iResult = HyperRecieveCommand(sockClient, &command);
    if (iResult != HYPER_SUCCESS)
    {
        printf("[-] HyperRecieveCommand failed\n");
        return HYPER_FAILED;
    }
    else
        printf("[+] Command recieved. %s\n", command);

    if (strcmp(command, "SEND") == 0)
    {
        HyperMemFree(command);

        printf("[+] Opening test file...\n");
        
        HYPERFILE fileBuffer = NULL;
        size_t fileSize = 0;

        printf("[+] Reading file into buffer...\n");
        iResult = HyperReadFile("test.png", &fileBuffer, &fileSize);
        if (iResult != HYPER_SUCCESS)
        {
            printf("[-] HyperReadFile failed\n");
            return HYPER_FAILED;
        }
       
        printf("[+] Sending file... File Size: %lu\n", fileSize);
        iResult = HyperSendFile(sockClient, &fileBuffer, fileSize);
        if (iResult != HYPER_SUCCESS)
        {
            printf("[-] HyperSendFile failed\n");
            return HYPER_FAILED;
        }
        else
            printf("[+] File sent successfully!\n");
       
        HyperMemFree(fileBuffer);
    }

    HyperCloseSocket(sockServer);
    HyperCloseSocket(sockClient);
    HyperSocketCleanup();
    return HYPER_SUCCESS;
}
