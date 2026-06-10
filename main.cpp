#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

int main()
{
    std::cout << "Starting High-Performance HTTP Server..." << std::endl;

    // 1. Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    // 2. Create the Socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed. Error: " << WSAGetLastError()
                  << std::endl;
        WSACleanup();
        return 1;
    }

    // 3. Bind the Socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) ==
        SOCKET_ERROR)
    {
        std::cerr << "Bind failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 4. Listen
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server successfully listening on port 8080..." << std::endl;

    // 5. The Accept Loop (Step 3 Addition)
    while (true)
    {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(
            serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed. Error: " << WSAGetLastError()
                      << std::endl;
            continue;  // Move to the next iteration instead of crashing the
                       // server
        }

        std::cout << "\n--- New Connection Accepted ---" << std::endl;

        // Read the incoming HTTP request
        const int BUFFER_SIZE = 4096;
        std::vector<char> buffer(BUFFER_SIZE);
        int bytesReceived =
            recv(clientSocket, buffer.data(), BUFFER_SIZE - 1, 0);

        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0';
            std::cout << "Received Request:\n" << buffer.data() << std::endl;
        }
        else if (bytesReceived == 0)
        {
            std::cout << "Client closed connection." << std::endl;
        }
        else
        {
            std::cerr << "Receive failed. Error: " << WSAGetLastError()
                      << std::endl;
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}