# http-client

## Tooling setup on macOS

### Compiler and basic tools

1. GET clang++ and standard header: run in your bash: `xcode-select --install`
2. Install CMake
    ```
    brew install cmake ninja
    ```

### Build and run on macOS:
1. From project root:
    ```
    mkdir -p build
    cd build

    cmake -G Ninja ..
    cmake --build .
    ```
2. We should have executable in build directory with `httpClient`. To make a stubbed request to `google.com/search?q=lyon+city`, run the executable with `./httpClient`.

## Notes:

### Socket:
- RAII: <b>Resource acqusation is intialization</b> is C++ religous practice, it says "tie the management of a resource with lifecycle of object."
- Tie the lifetime of socket file_descriptor to lifetime of TCPClient object. Destructor of TCPClient will release the socket file_descriptor even if exceptions occur.

### TCPClient:
- We have a iterator over returned addresses, because requested host can expose multiple Protocol Families (IPV4: `AF_INET` or IPV6: `AF_INET6`), Socket Types (TCP: `SOCK_STREAM` or UDP: `SOCK_DGRAM`). However, since we have mentioned `hints.ai_socktype = SOCK_STREAM` in TCPClient#connect(), getaddrinfo() function subject to restrictions imposed by hints.
- `Send` function of TCPClient tries to send all of data in buffer in first iteration of while loop, but as per <b>send buffer of the socket</b>, it breaks the buffer into chunks and iterate upto sent bytes are less then buffer length.
- Since data is recived in chunks and over time, we simply need to keep iterating `recv` function, until socket is closed by the server (`recv` returns `0`).

### HTTPClient:
- Since, `tcp_client_` is member variable of HttpClient, destruction of `http_client_` will invoke destructor of TcpClient, that inherntly calls `::close` on socket file_descriptor.
- In HTTP grammar,  \r\n\r\n represents two consecutive line-ends (Carriage Return + Line Feed), which serves as the "border" between the message headers and the message body.