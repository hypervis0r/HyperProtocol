#include "file.h"

int HyperRecieveFile(SOCKET sockServer, void *lpBuffer, unsigned long long *ullSize)
{
	unsigned long long ullFileSize = 0;
	unsigned long long ullWrittenSize = 0;
	unsigned long ulBytesWritten = 0;
	
    char cpSizeBuf[1024];
	char cpRecvBuffer[4096];
    memset(cpSizeBuf, 0, sizeof(cpSizeBuf));
    memset(cpRecvBuffer, 0, sizeof(cpRecvBuffer));

	// Recieve file size from server
	recv(sockServer, cpSizeBuf, sizeof(cpSizeBuf), 0);
	ullFileSize = strtoull(cpSizeBuf, 0, 10);

	// Set ullSize to ullFileSize plus the extra bit of data we add to the end.
	// We add the extra data because I suck at writing good code.
	*ullSize = ullFileSize + sizeof(cpRecvBuffer);

	// Recieve binary data from server, and write to buffer.
	while (ullWrittenSize < ullFileSize)
	{
		ulBytesWritten = recv(sockServer, cpRecvBuffer, sizeof(cpRecvBuffer), 0);

		// If we have recieved final bit of data, finish writing and break.
		if (ullWrittenSize > ullFileSize)
		{
			memcpy(lpBuffer + ullWrittenSize, cpRecvBuffer, ulBytesWritten);
            break;
		}
		else
		{
			memcpy(lpBuffer + ullWrittenSize, cpRecvBuffer, ulBytesWritten);
		    ullWrittenSize += ulBytesWritten;
			memset(cpRecvBuffer, 0, sizeof(cpRecvBuffer));
		}
	}

    return 0;
}


int HyperSendFile(SOCKET sockServer, void *lpBuffer, unsigned long long ullSize)
{
    unsigned long long ullSentSize = 0;
    unsigned long ulBytesSent = 0;
    char cpSendBuffer[4098];
    memset(cpSendBuffer, 0, sizeof(cpSendBuffer));

    while(ullSentSize <= ullSize)
    {
        memcpy(cpSendBuffer, lpBuffer + ullSentSize, sizeof(cpSendBuffer));
        ulBytesSent = send(sockServer, cpSendBuffer, sizeof(cpSendBuffer), 0);
        if (ulBytesSent == SOCKET_ERROR)
            return SOCKET_ERROR;
        else
        {
            ullSentSize += ulBytesSent;
            memset(cpSendBuffer, 0, sizeof(cpSendBuffer));
        }
    }

    return 0;
}
