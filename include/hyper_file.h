#ifndef _FILE_H
#define _FILE_H

#include "hyper_network.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Size of each input chunk to be
   read and allocate for. */
#ifndef  READALL_CHUNK
#define  READALL_CHUNK  262144
#endif

#define  READALL_OK          0  /* Success */
#define  READALL_INVALID    -1  /* Invalid parameters */
#define  READALL_ERROR      -2  /* Stream error */
#define  READALL_TOOMUCH    -3  /* Too much input */
#define  READALL_NOMEM      -4  /* Out of memory */


int HyperReadFile(FILE *in, void **dataptr, size_t *sizeptr);
int HyperRecieveFile(SOCKET sockServer, void **lpBuffer, unsigned long *ulSize);
int HyperSendFile(SOCKET sockServer, void *lpBuffer, unsigned long ulSize);

#endif
