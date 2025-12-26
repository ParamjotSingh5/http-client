
#include "HttpClient.hpp"
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
    HttpClient http_client_;

    if(http_client_.connect(host, "80")){
        cout << "Conected to " << host << ". Sending GET request." << endl;

        std::string response = http_client_.get(path);

        if(!response.empty()){
            size_t header_end = response.find("\r\n\r\n");
            size_t body_size = (header_end != string::npos) ? response.length() - (header_end + 4) : 0;

            cout << "Request Received " << response.length() << "total bytes (" 
                << body_size << " body bytes)." << endl;
        }
        else {
             cerr << "Request Failed to receive a response." << endl;
        }

    }else {
        cerr << "Request Failed to connect to " << host << endl;
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