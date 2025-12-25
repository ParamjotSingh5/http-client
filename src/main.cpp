// src/main.cpp - Sequential/Single-Threaded Test
#include "TcpClient.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using namespace std::chrono;

// --- Sequential Fetch Function (No threading) ---
void sendHTTPRequest(const string& host, const string& path, int id) {
    TcpClient client;
    
    // Attempt to connect to the server (HTTP default port 80)
    if (client.connect(host, "80")) {

        // 1. Construct the HTTP Request String
        std::stringstream request;
        
        // a. Request Line: GET /path HTTP/1.1
        request << "GET " << path << " HTTP/1.1\r\n";
        
        // b. Host Header (Mandatory in HTTP/1.1):
        request << "Host: " << host << "\r\n";
        
        // c. Connection Header (Tells server to close socket after response, simplifying our receive logic)
        request << "Connection: close\r\n";
        
        // d. Empty Line (Signals the end of headers, CRUCIAL for the protocol)
        request << "\r\n"; 

        string full_request = request.str();
        
        // 2. Send Request
        if (!client.send(full_request)) {
            cerr << "Failed to send HTTP request." << endl;
            return;
        }
        
        // 3. Receive Response
        // We rely on the TcpClient::receive() function to read until the server closes the connection.
        string raw_response = client.receive();
        
        // 4. Basic Error Check
        if (raw_response.empty()) {
            cerr << "Received empty response or error during receive." << endl;
            return;
        }

        cout << "response:" << "/r/n" << raw_response << endl;
        
        
        client.close();
    } else {
        cerr << "[Request " << id << "] ❌ Failed to connect to " << host << endl;
    }
}

int main() {
    const string TARGET_HOST = "google.com";
    const string TARGET_PATH = "/search?q=lyon+city";

    cout << "--- Connecting TCP client ---" << endl;
    
    // --- Performance Timing Start ---
    auto start_time = high_resolution_clock::now();

    sendHTTPRequest(TARGET_HOST, TARGET_PATH, 1);
    cout << "---" << endl;
    

    // --- Performance Timing End ---
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_time - start_time);

    cout << "------------------------------------------------------------------" << endl;

    return 0;
}