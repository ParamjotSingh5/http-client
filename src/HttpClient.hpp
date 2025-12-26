#include "TcpClient.hpp"


class HttpClient
{
private:
    TcpClient tcp_client_;
    std::string host_;


public:
    bool connect(const std::string& node, const std::string& service){
        host_ = node;
        return tcp_client_.connect(node, service);
    }
    
    std::string get(const std::string& path);
    // We can extend the HTTP client by support other available verbs

};
