# HyperProtocol

The Hyper Protocol is a library for C that allows for sending and receiving files over the internet in a fast, low-overhead, platform-agnostic way.

## Installation

Use `make` to generate the library file, then link library file and headers into your project.

## Usage

```c
#include <hyper.h>

int main(void)
{
    SOCKET server = 0;    
    HYPERFILE buffer = NULL;
    size_t file_size = 0;

    /* Initialize Hyper Network Functions */
    HyperNetworkInit(); 

    /* Connect to server */
    HyperConnectServer(&server, "127.0.0.1", 2090); 
    
    /* Read file into buffer */
    HyperReadFile("example_file.txt", &buffer, &file_size);
    
    /* Send file over network */
    HyperSendFile(server, buffer, file_size);

    /* Free the buffer */
    HyperMemFree(buffer);

    return 0;
}
```
Further documentation can be located [here](https://hypervis0r.github.io/HyperProtocol/)

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License
[GNU GPLv2](https://choosealicense.com/licenses/gpl-2.0/)
