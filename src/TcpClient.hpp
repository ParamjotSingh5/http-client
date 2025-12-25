// src/TcpClient.hpp
#include <string>
#include <unistd.h> // For close()
#include <sys/socket.h> // For socket(), connect()
#include <netdb.h> // For getaddrinfo()

class TcpClient {
public:
    TcpClient() : socket_fd_(-1) {}
    ~TcpClient() { close(); } // Use RAII principle
    
    bool connect(const std::string& host, const std::string& port);
    void close();

private:
    int socket_fd_;
};