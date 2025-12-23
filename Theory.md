# HTTP Client in C++

- Design:
    - Sockets contract (BSD sockets interface) are implemented by OS vendors and they manages Socket resources such as Protocol type (TCP, UDP), local or remote IP addresses, post number, connection state and data-buffers for transactions.
    - We declare functions offered by our TCP client in a `TCP.hpp` file. As a prerequisite, a socket file_descriptor (an integer value, can be used as a ticket while commanding OS to perform some standard operations, such as `write`, `read`, `close`) is defined in this contract.
    - `TCP.cpp` is the definition of our TCP client, that can be further used by a HTTP, RPC clients. `TCP.cpp` is the layer that delegates the connection request to a socket, by using file_descriptor of socket.
- Everything in UNIX is a file:
    - Most OS have different rules for different operations, one way to talk to Disk, other way to keyboard. Unix treats every resource as a stream of bytes.
    - When we **Open** something in Unix, kernel just gives us a non-negative integer (a File Descriptor or `fd`). We can use this `fd` as a identifier, while commanding operations on opened resource.
        - **`open()`**: Get a claim ticket (FD).
        - **`read()`**: Get bytes from the ticket.
        - **`write()`**: Send bytes to the ticket.
        - **`close()`**: Give the ticket back.
    
    ```java
    process
     └── fd table
          3 ──► kernel object
    ```
    
    - We will create a **Socket**. The OS will give us an integer (the FD).
    - If we want to send an HTTP Request, we don't use a special "NetworkSend" command; we just use `write(socket_fd, "GET / HTTP/1.1...", length)`.
    - `FD` points to a kernel object, it might represent a “regular file”, “pipe”, “socket”, "character device”, “epoll instance”. The kernel object resolves to a `struct file`, that contains:
        - a pointer to **file operations**
        - a pointer to **private data** (socket state, address)
        
        ```java
        struct file {
            const struct file_operations *f_op;
            void *private_data;
        }
        ```
        
    - When we call `write()`, with a `fd`, kernel calls `file->f_op->write(file, buf, len);`
- Anatomy of a Socket:
    - To create a connection we need 3 things defined in <sys/socket.h> lib:
    - The Socket Creation (`socket()`)
        
        ```java
        int sockfd = socket(int domain, int type, int protocol);
        //int fd = socket(AF_INET, SOCK_STREAM, 0);
        ```
        
        - `domain` (or `family`): Specifies the address family.
            - `AF_INET`: IPv4 protocol.
            - `AF_INET6`: IPv6 protocol.
            - `AF_UNIX` or `AF_LOCAL`: Local communication (within the same system).
        - `type`: Specifies the socket type.
            - `SOCK_STREAM`: For TCP (connection-oriented, reliable stream, can be slow against UDP).
            - `SOCK_DGRAM`: For UDP (connectionless, may lose packets, efficient against TCP).
        - `protocol`: Specifies the protocol to use. Usually set to `0`, allowing the system to choose the default protocol (usually IP).
    - The Address Structure (`sockaddr_in`)
        - Before making a request, we need to know who we are talking with. We use structure called `sockaddr_in` to store the IP address and the port of server.
    - The Connection (`connect()`)
        - Socket’s `connect` initiates the 3-way handshake. Returns `0` means connection is open for further communication.