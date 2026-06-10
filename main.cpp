#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>
#include <csignal>     
#include <atomic>   
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") 

std::atomic<bool> serverRunning(true);
SOCKET globalServerSocket = INVALID_SOCKET;

void handleExitSignal(int signum) {
    std::cout << "\n[Interrupt " << signum << "] Initiating graceful shutdown..." << std::endl;
    serverRunning = false;

    if (globalServerSocket != INVALID_SOCKET) {
        closesocket(globalServerSocket);
    }
}

void handleConnection(SOCKET clientSocket) {
    const int BUFFER_SIZE = 4096;
    std::vector<char> buffer(BUFFER_SIZE);
    int bytesReceived = recv(clientSocket, buffer.data(), BUFFER_SIZE - 1, 0);

    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0'; 
        
        std::string request(buffer.data());
        std::istringstream iss(request);
        std::string method, uri, version;
        
        iss >> method >> uri >> version;
        
        std::cout << "[Thread " << std::this_thread::get_id() << "] Requested URI: " << uri << std::endl;

        std::string filePath = (uri == "/") ? "index.html" : uri.substr(1) + ".html";
        std::ifstream file(filePath, std::ios::binary);
        std::string response;

        if (file.is_open()) {
            std::ostringstream fileStream;
            fileStream << file.rdbuf();
            std::string body = fileStream.str();

            response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: text/html\r\n";
            response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
            response += "Connection: close\r\n\r\n"; 
            response += body;
        } else {
            std::string body = "<h1>404 - File Not Found</h1>";
            response = "HTTP/1.1 404 Not Found\r\n";
            response += "Content-Type: text/html\r\n";
            response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
            response += "Connection: close\r\n\r\n";
            response += body;
        }

        send(clientSocket, response.c_str(), response.size(), 0);
    }

    closesocket(clientSocket);
}


int main() {
    signal(SIGINT, handleExitSignal);

    std::cout << "Starting High-Performance HTTP Server..." << std::endl;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    globalServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (globalServerSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(8080);       

    if (bind(globalServerSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(globalServerSocket);
        WSACleanup();
        return 1;
    }

    if (listen(globalServerSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(globalServerSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server successfully listening on port 8080. Press Ctrl+C to stop." << std::endl;

    while (serverRunning) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        
        SOCKET clientSocket = accept(globalServerSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        
        if (clientSocket == INVALID_SOCKET) {
            // If accept() failed because our signal handler closed the socket, break the loop naturally
            if (!serverRunning) {
                break;
            }
            continue; 
        }

        std::thread(handleConnection, clientSocket).detach();
    }

    std::cout << "Cleaning up Winsock resources..." << std::endl;
    WSACleanup();
    std::cout << "Server stopped cleanly. Goodbye!" << std::endl;
    
    return 0;
}