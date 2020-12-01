#ifndef _FILE_H
#define _FILE_H

#include "network.h"
#include <string.h>
#include <stdlib.h>

int HyperRecieveFile(SOCKET sockServer, void *lpBuffer, unsigned long long *ullSize);
int HyperSendFile(SOCKET sockServer, void *lpBuffer, unsigned long long ullSize);

#endif
