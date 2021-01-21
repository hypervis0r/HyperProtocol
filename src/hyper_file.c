#include <hyper/hyper_file.h>

/* Lol I stole all of the readall code from SO */
/* This function returns one of the READALL_ constants above.
   If the return value is zero == READALL_OK, then:
     (*dataptr) points to a dynamically allocated buffer, with
     (*sizeptr) chars read from the file.
     The buffer is allocated for one extra char, which is NUL,
     and automatically appended after the data.
   Initial values of (*dataptr) and (*sizeptr) are ignored.
*/
HYPERSTATUS 
HyperReadFileC(
    FILE                *in, 
    void                **dataptr, 
    size_t              *sizeptr)
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

HYPERSTATUS 
HyperReceiveFile(
    const SOCKET        sockServer, 
    void                **lpBuffer, 
    unsigned long       *ulSize)
{
    HYPERSTATUS iResult = 0;

    unsigned long ulFileSize = 0;
	unsigned long ulWrittenSize = 0;
	unsigned long ulBytesWritten = 0;
    void *data = NULL;

    char cpSizeBuf[1024];
    memset(cpSizeBuf, 0, sizeof(cpSizeBuf));

	// Recieve file size from server
	recv(sockServer, cpSizeBuf, sizeof(cpSizeBuf), 0);
	ulFileSize = strtoull(cpSizeBuf, 0, 10);

    // If data is less than a block size long, lets not bloat the size lol
    if (ulFileSize < RECV_BLOCK_SIZE)
    {
        HyperMemAlloc(&data, ulFileSize);
        
        recv(sockServer, (char*)data, ulFileSize, 0);
        
        *lpBuffer = data;
        *ulSize = ulFileSize;
        return HYPER_SUCCESS;
    }

    // Allocate data buffer
    iResult = HyperMemAlloc(&data, ulFileSize + RECV_BLOCK_SIZE);
    if (iResult == HYPER_FAILED)
        return HYPER_FAILED;

	// Recieve binary data from server, and write to buffer.
	while (ulWrittenSize < ulFileSize)
	{
        ulBytesWritten = recv(sockServer, (char*)(data) + ulWrittenSize, RECV_BLOCK_SIZE, 0);

		// If we have recieved final bit of data, finish writing and break.
		if (ulWrittenSize > ulFileSize)
            break;
		else
		    ulWrittenSize += ulBytesWritten;
	}

    // Set input buffer to recieved data.
    *lpBuffer = data;
    
    // Set ullSize to ullFileSize plus the extra bit of data we add to the end.
	// We add the extra data because I suck at writing good code.
	*ulSize = ulFileSize + RECV_BLOCK_SIZE;

    return 0;
}

HYPERSTATUS
HyperReadFile(
    const char          *cpFilePath,
    HYPERFILE           *lpBuffer,
    size_t              *lpFileSize)
{
    HYPERSTATUS iResult = 0;
    HYPERFILE data = NULL;
    size_t stBytesRead = 0;
    
#ifdef _WIN32
    HANDLE hFile = 0;
    LARGE_INTEGER liFileSize = {0};
    DWORD dwLength = 0;

    hFile = CreateFileA(
            cpFilePath,     /* lpFileName */ 
            GENERIC_READ,   /* dwDesiredAccess */
            0,              /* dwShareMode */
            0,              /* lpSecurityAttributes */
            OPEN_EXISTING,  /* dwCreationDisposition */
            FILE_ATTRIBUTE_READONLY,    /* dwFlagsAndAttributes */
            0               /* hTemplateFile */
    );
    if (hFile == NULL)
        return HYPER_FAILED;

    /* Get file size */
    iResult = GetFileSizeEx(hFile, &liFileSize);
    if (iResult == NULL)
    {
        CloseHandle(hFile);
        return HYPER_FAILED;
    }

    dwLength = liFileSize.QuadPart;

    /* Allocate Memory */
    iResult = HyperMemAlloc(&data, dwLength);
    if (iResult == HYPER_FAILED)
    {
        CloseHandle(hFile);
        return HYPER_FAILED;
    }

    /* Read file into buffer */
    iResult = ReadFile(
            hFile,          /* hFile */
            data,           /* lpBuffer */
            dwLength,       /* nNumberOfBytesToRead */
            &stBytesRead,   /* lpNumberOfBytesRead */
            0               /* lpOverlapped */
    );
    if (iResult == FALSE)
    {
        CloseHandle(hFile);
        HyperMemFree(data);
        return HYPER_FAILED;
    }
    
    CloseHandle(hFile);
#else
    struct stat st = {0};
    int filesize = 0;
    int fd = 0;

    /* Get File Size */
    iResult = stat(cpFilePath, &st);
    if (iResult == -1)
        return HYPER_FAILED;

    filesize = st.st_size;

    /* Open File Descriptor */
    fd = open(cpFilePath, O_RDONLY);
    if (fd == -1)
        return HYPER_FAILED;

    /* Allocate buffer */
    iResult = HyperMemAlloc(&data, filesize);
    if (iResult == HYPER_FAILED)
        return HYPER_FAILED;

    /* Read file into buffer */
    stBytesRead = read(fd, data, filesize);
    if ((int)stBytesRead == -1)
    {
        close(fd);
        HyperMemFree(data);
        return HYPER_FAILED;
    }

    close(fd);
#endif
    
    /* Set HYPERFILE and size_t parameter */
    *lpBuffer = data;
    *lpFileSize = stBytesRead;

    return HYPER_SUCCESS;
}

HYPERSTATUS
HyperWriteFileC(
    FILE                *pFile,
    const HYPERFILE     hfBuffer,
    const size_t        stFileSize
)
{
    HYPERSTATUS hsResult = 0;
    
    if (pFile)
    {
        hsResult = fwrite(hfBuffer, stFileSize, 1, pFile);
        if (hsResult < 1)
            return HYPER_FAILED;
    }

    return HYPER_SUCCESS;
}

HYPERSTATUS
HyperWriteFile(
    const char          *cpFilePath,
    const HYPERFILE     hfBuffer,
    const size_t        stFileSize
)
{
    HYPERSTATUS hsResult = 0;

#ifdef _WIN32
    HANDLE hFile = NULL;
    DWORD dwBytesWritten = 0;

    hFile = CreateFileA(
            cpFilePath,     /* lpFileName */ 
            GENERIC_READ,   /* dwDesiredAccess */
            0,              /* dwShareMode */
            0,              /* lpSecurityAttributes */
            CREATE_ALWAYS,  /* dwCreationDisposition */
            FILE_ATTRIBUTE_NORMAL,    /* dwFlagsAndAttributes */
            0               /* hTemplateFile */
    );
    if (hFile == NULL)
        return HYPER_FAILED;

    hsResult = WriteFile(
            hFile,          /* hFile */
            hfBuffer,       /* lpBuffer */
            stFileSize,     /* nNumberOfBytesToWrite */
            &dwBytesWritten,    /* lpNumberOfBytesWritten */
            NULL            /* lpOverlapped */
    );
    if (hsResult == NULL)
    {
        CloseHandle(hFile);
        return HYPER_FAILED;
    }

    CloseHandle(hFile);
#else
    int fd = 0;
    
    fd = open(cpFilePath, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return HYPER_FAILED;

    hsResult = write(fd, hfBuffer, stFileSize);
    if (hsResult == -1)
    {
        close(fd);
        return HYPER_FAILED;
    }

    close(fd);
#endif

    return HYPER_SUCCESS;
}

HYPERSTATUS 
HyperSendFile(
    const SOCKET        sockServer, 
    HYPERFILE           *lpBuffer, 
    const unsigned long ulSize)
{
    HYPERSTATUS iResult = 0;
    unsigned long ulSentSize = 0;
    unsigned long ulBytesSent = 0;
    
    char fileSizeBuffer[FILESIZE_BUFFER_SIZE];
    memset(fileSizeBuffer, 0, FILESIZE_BUFFER_SIZE);

    // Add extra block due to memcpy scaring me
    iResult = HyperMemRealloc(lpBuffer, ulSize + SEND_BLOCK_SIZE);
    if (iResult == HYPER_FAILED)
        return HYPER_FAILED;

    // Send File Size to Peer
    snprintf(fileSizeBuffer, FILESIZE_BUFFER_SIZE, "%lu", ulSize);
    ulBytesSent = send(sockServer, fileSizeBuffer, FILESIZE_BUFFER_SIZE, 0);
    if ((int)ulBytesSent == SOCKET_ERROR)
        return SOCKET_ERROR;
  
    // If data is less than a block size long, lets not bloat the size lol
    if (ulSize < SEND_BLOCK_SIZE)
    {
        send(sockServer, (char*)(*lpBuffer), ulSize, 0);
        return HYPER_SUCCESS;
    }

    // Begin Sending file
    while(ulSentSize < ulSize)
    {
        // Copy data from buffer into block
        ulBytesSent = send(sockServer, (char*)(*lpBuffer)+ulSentSize, SEND_BLOCK_SIZE, 0);
        
        if ((int)ulBytesSent == SOCKET_ERROR)
            return SOCKET_ERROR;
        else
            ulSentSize += ulBytesSent;
    }
    
    return HYPER_SUCCESS;
}
