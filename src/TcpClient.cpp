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

    // Define attributes values to filter conections we are willing to support and connect. The getaddrinfo function will use these values while resolving available addresses on host.
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; // We  TCP contract 

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

// --- Send Function ---
bool TcpClient::send(const string& data) {
    if (socket_fd_ == -1) {
        cerr << "Error: Cannot send data, socket is not connected." << endl;
        return false;
    }

    const char* buffer = data.c_str();
    size_t total_sent = 0;
    size_t data_len = data.length();
    int bytes_sent;

    // Loop until all bytes have been sent (this is crucial for reliability)
    while (total_sent < data_len) {
        // The send() function attempts to send data on the socket.
        bytes_sent = ::send(socket_fd_, buffer + total_sent, data_len - total_sent, 0);

        if (bytes_sent == -1) {
            cerr << "Error sending data: " << strerror(errno) << endl;
            return false;
        }
        if (bytes_sent == 0) {
            // Should not happen on a blocking TCP socket unless connection is closed prematurely
            cerr << "Warning: Zero bytes sent, connection likely closed." << endl;
            return false;
        }

        total_sent += bytes_sent;
    }

    return true;
}

// --- Receive Function (Reads until connection closes) ---
string TcpClient::receive(size_t max_bytes) {
    if (socket_fd_ == -1) {
        cerr << "Error: Cannot receive data, socket is not connected." << endl;
        return "";
    }

    std::string received_data;
    std::vector<char> buffer(max_bytes);
    int bytes_read;

    // Loop to read data chunks until the connection is closed (bytes_read == 0)
    while ((bytes_read = ::recv(socket_fd_, buffer.data(), buffer.size(), 0)) > 0) {
        // Append the received chunk to our result string
        received_data.append(buffer.data(), bytes_read);
    }

    if (bytes_read == -1) {
        // An error occurred during reading (e.g., connection reset)
        cerr << "Error receiving data: " << strerror(errno) << endl;
        return "";
    }

    // If bytes_read == 0, the server gracefully closed the connection, which is the expected
    // end of the response stream for a basic HTTP/1.1 client using Connection: close.
    return received_data;
}