// src/TcpClient.cpp
#include "TcpClient.hpp"
#include <iostream>
#include <cstring>
#include <sys/types.h> // Required for getaddrinfo on some systems
#include <sys/socket.h> // For send(), recv()

using std::string;
using std::cerr;
using std::endl;
using std::cout;

// --- Close Function (RAII) ---
void TcpClient::close() {
    if (socket_fd_ != -1) {
        ::close(socket_fd_); // Use ::close to explicitly call the POSIX function
        socket_fd_ = -1;
    }
}

// --- Connect Function (Core Logic) ---
bool TcpClient::connect(const string& host, const string& port) {
    // 1. Setup hints for getaddrinfo
    struct addrinfo hints = {0}; // Sets everything to zero immediately
    struct addrinfo *resolvedAddressInfoPtr; // The getaddrinfo will prepare a LinkedList of type addrinfo in heap. 
    // then it needs to tell us the memory address of the first item in that list.
    // To change the value of the pointer resolvedAddressInfoPtr so that it points to the new memory location, the function needs the address of your pointer.

    // 2. Resolve Hostname to IP Address and Port
    // getaddrinfo populates server_info with a linked list of possible addresses.
    int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &resolvedAddressInfoPtr);
    if (status != 0) {
        cerr << "getaddrinfo error: " << gai_strerror(status) << endl;
        return false;
    }
    

    // 3. Loop Through Results and Connect
    for (addrinfo *p = resolvedAddressInfoPtr; p != NULL; p = p->ai_next) {
        // a. Create Socket: socket(address family, socket type, protocol)
        socket_fd_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_fd_ == -1) {
            // Couldn't create this socket type, try next one
            continue; 
        }

        // b. Connect: Attempt the three-way handshake
        if (::connect(socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
            ::close(socket_fd_);
            socket_fd_ = -1;
            continue; // Connection failed, try next address
        }
        
        // c. Success: If we reach here, we connected successfully
        break; 
    }

    // 4. Cleanup and Final Check
    freeaddrinfo(resolvedAddressInfoPtr); // getaddrinfo dynamically allocates memory for that linked list of addresses, you must call this to prevent memory leaks once you are done with the results.
    

    cout << "--- TCP client connected ---" << endl;

    // Success!
    return true;
}