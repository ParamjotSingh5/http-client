// src/main.cpp - Sequential/Single-Threaded Test
#include "TcpClient.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using namespace std::chrono;

// --- Sequential Fetch Function (No threading) ---
void connectSocket(const string& host, const string& path, int id) {
    TcpClient client;
    
    // Attempt to connect to the server (HTTP default port 80)
    if (client.connect(host, "80")) {
        
        client.close();
    } else {
        cerr << "[Request " << id << "] ❌ Failed to connect to " << host << endl;
    }
}

int main() {
    const string TARGET_HOST = "google.com";
    const string TARGET_PATH = "/";

    cout << "--- Connecting TCP client ---" << endl;
    
    // --- Performance Timing Start ---
    auto start_time = high_resolution_clock::now();

    connectSocket(TARGET_HOST, TARGET_PATH, 1);
    cout << "---" << endl;
    

    // --- Performance Timing End ---
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_time - start_time);

    cout << "------------------------------------------------------------------" << endl;

    return 0;
}