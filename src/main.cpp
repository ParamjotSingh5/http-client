
#include "HttpClient.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>

using std::string;
using std::cout;
using std::cerr;
using std::cin;
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

// Helper to split string by space (e.g., "example.com /index.html" -> ["example.com", "/index.html"])
std::vector<string> split_input(const string& str) {
    std::vector<string> tokens;
    std::stringstream ss(str);
    string token;
    while (ss >> token) { //stream extraction operator, defaults to read characters until it hits whitespace (space, tab, or newline).
        tokens.push_back(token);
    }
    return tokens;
}

/**
 * Verifies if the host is a pure hostname.
 * Returns true if valid, false if it contains path/query elements.
 */
bool is_valid_host(const std::string& host, std::string& error_msg) {
    if (host.empty()) {
        error_msg = "Host cannot be empty.";
        return false;
    }

    // Characters that indicate the start of a path, query, or fragment
    // according to RFC 3986.
    const std::string illegal_chars = "/?#";

    size_t found = host.find_first_of(illegal_chars);
    
    if (found != std::string::npos) {
        char offending_char = host[found];
        error_msg = "Invalid format! Host contains '" + std::string(1, offending_char) + "'.";
        error_msg += "\nExpected format: 'google.com', not 'google.com" + host.substr(found) + "'";
        return false;
    }

    // Optional: Check for protocol prefixes if user types "http://google.com"
    if (host.find("://") != std::string::npos) {
        error_msg = "Invalid format! Do not include 'http://' or 'https://'. Just provide the hostname.";
        return false;
    }

    return true;
}

int main() {
    cout << "============================================" << endl;
    cout << "          C++ HTTP Client Playground        " << endl;
    cout << "============================================" << endl;
    cout << "Usage: Enter '<host> <path>'" << endl;
    cout << "Example: example.com /" << endl;
    cout << "Type 'exit' or 'quit' to close the program." << endl;
    cout << "============================================" << endl;
    
    string input_line;

    // --- The Infinite Loop (REPL) ---
    while (true) {
        cout << "\nhttp-client> ";

        if (!std::getline(cin, input_line)) {
            break; // Handle Ctrl+D or stream error
        }

        if (input_line == "exit" || input_line == "quit" || input_line.empty()) {
            if (input_line.empty()) continue;
            break;
        }

        std::vector<string> args = split_input(input_line);
        string target_host = args[0];
        string error_msg;
                
        // --- VALIDATION STEP ---
        if (!is_valid_host(target_host, error_msg)) {
            cerr << "[!] " << error_msg << endl;
            continue;
        }

        string target_path = (args.size() > 1) ? args[1] : "/";



        cout << "[*] Connecting to " << target_host << "..." << endl;
        
        HttpClient client; 
        
        if (client.connect(target_host, "80")) {
            cout << "[*] Sending GET " << target_path << "..." << endl;
            
            std:string response = client.get(target_path);
            
            if (response.length()  != 0) {
                cout << "--------------------------------------------" << endl;
                cout << "SIZE:   " << response.size() << " bytes" << endl;
                cout << "--------------------------------------------" << endl;
                
            } else {
                cerr << "[!] Error: Empty or invalid response." << endl;
            }
        } else {
            cerr << "[!] Error: Could not connect to host." << endl;
        }
    }

    cout << "Exiting. Goodbye!" << endl;
    return 0;
}