#include "hyper_file.h"

/* Lol I stole all of the readall code from SO */

/* This function returns one of the READALL_ constants above.
   If the return value is zero == READALL_OK, then:
     (*dataptr) points to a dynamically allocated buffer, with
     (*sizeptr) chars read from the file.
     The buffer is allocated for one extra char, which is NUL,
     and automatically appended after the data.
   Initial values of (*dataptr) and (*sizeptr) are ignored.
*/
int HyperReadFile(FILE *in, void **dataptr, size_t *sizeptr)
{
    char  *data = NULL, *temp;
    size_t size = 0;
    size_t used = 0;
    size_t n;

    /* None of the parameters can be NULL. */
    if (in == NULL || dataptr == NULL || sizeptr == NULL)
        return READALL_INVALID;

    /* A read error already occurred? */
    if (ferror(in))
        return READALL_ERROR;

    while (1) {

        if (used + READALL_CHUNK + 1 > size) {
            size = used + READALL_CHUNK + 1;

            /* Overflow check. Some ANSI C compilers
               may optimize this away, though. */
            if (size <= used) {
                free(data);
                return READALL_TOOMUCH;
            }

            temp = realloc(data, size);
            if (temp == NULL) {
                free(data);
                return READALL_NOMEM;
            }
            data = temp;
        }

        n = fread(data + used, 1, READALL_CHUNK, in);
        if (n == 0)
            break;

        used += n;
    }

    if (ferror(in)) {
        free(data);
        return READALL_ERROR;
    }

    temp = realloc(data, used + 1);
    if (temp == NULL) {
        free(data);
        return READALL_NOMEM;
    }
    data = temp;
    data[used] = '\0';

    *dataptr = data;
    *sizeptr = used;

    return READALL_OK;
}

int HyperRecieveFile(SOCKET sockServer, void **lpBuffer, unsigned long *ulSize)
{
    unsigned long ulFileSize = 0;
	unsigned long ulWrittenSize = 0;
	unsigned long ulBytesWritten = 0;
    void *data = NULL;

    char cpSizeBuf[1024];
	char cpRecvBuffer[4096];
    memset(cpSizeBuf, 0, sizeof(cpSizeBuf));
    memset(cpRecvBuffer, 0, sizeof(cpRecvBuffer));

	// Recieve file size from server
	recv(sockServer, cpSizeBuf, sizeof(cpSizeBuf), 0);
	ulFileSize = strtoull(cpSizeBuf, 0, 10);

    // Allocate data buffer
    data = realloc(data, ulFileSize + sizeof(cpRecvBuffer));
    if (data == NULL) 
        return -1;

	// Recieve binary data from server, and write to buffer.
	while (ulWrittenSize < ulFileSize)
	{
        printf("// Size Recieved: %lu //\n", ulWrittenSize);
		ulBytesWritten = recv(sockServer, cpRecvBuffer, sizeof(cpRecvBuffer), 0);

		// If we have recieved final bit of data, finish writing and break.
		if (ulWrittenSize > ulFileSize)
		{
			memcpy(data + ulWrittenSize, cpRecvBuffer, ulBytesWritten);
            break;
		}
		else
		{
			memcpy(data + ulWrittenSize, cpRecvBuffer, ulBytesWritten);
		    ulWrittenSize += ulBytesWritten;
			memset(cpRecvBuffer, 0, sizeof(cpRecvBuffer));
		}
	}

    *lpBuffer = data;
    
    // Set ullSize to ullFileSize plus the extra bit of data we add to the end.
	// We add the extra data because I suck at writing good code.
	*ulSize = ulFileSize + sizeof(cpRecvBuffer);

    return 0;
}

int HyperSendFile(SOCKET sockServer, void *lpBuffer, unsigned long ulSize)
{
    unsigned long ulSentSize = 0;
    unsigned long ulBytesSent = 0;
    char cpSendBuffer[4098];
    char cpSizeBuffer[1024];
    memset(cpSendBuffer, 0, sizeof(cpSendBuffer));
    memset(cpSizeBuffer, 0, sizeof(cpSizeBuffer));

    // Add extra block due to memcpy scaring me
    lpBuffer = realloc(lpBuffer, ulSize + sizeof(cpSendBuffer));

    // Send File Size to Peer
    snprintf(cpSizeBuffer, sizeof(cpSizeBuffer), "%lu", ulSize);
    ulBytesSent = send(sockServer, cpSizeBuffer, sizeof(cpSizeBuffer), 0);
    if (ulBytesSent == SOCKET_ERROR)
        return SOCKET_ERROR;
   
    // Begin Sending file
    while(ulSentSize < ulSize)
    {
        memcpy(cpSendBuffer, lpBuffer + ulSentSize, sizeof(cpSendBuffer));
        ulBytesSent = send(sockServer, cpSendBuffer, sizeof(cpSendBuffer), 0);
        if (ulBytesSent == SOCKET_ERROR)
            return SOCKET_ERROR;
        else
        {
            ulSentSize += ulBytesSent;
            memset(cpSendBuffer, 0, sizeof(cpSendBuffer));
        }
    }

    return 0;
}
