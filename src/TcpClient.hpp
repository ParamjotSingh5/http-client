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
    bool send(const std::string& data);
    std::string receive(size_t max_bytes = 4096);
    void close();

private:
    int socket_fd_;
};