#include "HttpClient.hpp"
#include <iostream>
#include <sstream>

using std::string;
using std::cerr;
using std::endl;

string HttpClient::get(const string& path){
    if(host_.empty()){
        cerr << "Error: HttpClient not connected. Call connect() first." << endl;
        return "";
    }

    // 1. Construct the HTTP Request String
    std::stringstream request;
    
    // a. Request Line: GET /path HTTP/1.1
    request << "GET " << path << " HTTP/1.1\r\n";
    
    // b. Host Header (Mandatory in HTTP/1.1):
    request << "Host: " << host_ << "\r\n";
    
    // c. Connection Header (Tells server to close socket after response, simplifying our receive logic)
    request << "Connection: close\r\n";
    
    // d. Empty Line (Signals the end of headers, CRUCIAL for the protocol)
    request << "\r\n"; 

    string full_request = request.str();
    
    // 2. Send Request
    if (!tcp_client_.send(full_request)) {
        cerr << "Failed to send HTTP request." << endl;
        return "";
    }
    
    // 3. Receive Response
    // We rely on the TcpClient::receive() function to read until the server closes the connection.
    string raw_response = tcp_client_.receive();
    
    // 4. Basic Error Check
    if (raw_response.empty()) {
        cerr << "Received empty response or error during receive." << endl;
        return "";
    }

    // **Optional:** Here you would call parse_response() to extract the body and headers.
    // For this basic playground, we will return the full raw string for now.
    
    // 

    return raw_response;
}