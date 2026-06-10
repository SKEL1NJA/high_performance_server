#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") 

int main() {
    std::cout << "Starting High-Performance HTTP Server..." << std::endl;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(8080);       

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server successfully listening on port 8080..." << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            continue; 
        }

        const int BUFFER_SIZE = 4096;
        std::vector<char> buffer(BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer.data(), BUFFER_SIZE - 1, 0);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; 
            
            // --- NEW: Parsing Logic ---
            std::string request(buffer.data());
            std::istringstream iss(request);
            std::string method, uri, version;
            
            // The string stream automatically splits by spaces
            iss >> method >> uri >> version;
            
            std::cout << "\n--- Parsed Request ---" << std::endl;
            std::cout << "Method:  " << method << std::endl;
            std::cout << "URI:     " << uri << std::endl;
            std::cout << "Version: " << version << std::endl;
            std::cout << "----------------------\n" << std::endl;
            
        } else if (bytesReceived == 0) {
            std::cout << "Client closed connection." << std::endl;
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}