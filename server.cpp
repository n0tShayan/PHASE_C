#include "crypto.h"
#include "server_utils.h"
#include "file_utils.h"
#include <iostream>
#include <thread>
#include <winsock2.h>
#include <windows.h>
#include <iomanip>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

ServerUtils serverUtils;
LatticeCrypto crypto(101);

// Windows Console Color Utility
void setConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// GUI Functions
void printBanner() {
    setConsoleColor(11); // Cyan
// Updated ASCII Art for Server and Client with proper escape sequences
std::cout << "\033[1;36m" << R"(

  _____  _    _           _____ ______             _____ 
 |  __ \| |  | |   /\    / ____|  ____|           / ____|
 | |__) | |__| |  /  \  | (___ | |__             | |     
 |  ___/|  __  | / /\ \  \___ \|  __|            | |     
 | |    | |  | |/ ____ \ ____) | |____           | |____ 
 |_|    |_|  |_/_/    \_\_____/|______|           \_____|
                                         ______          
                                        |______|         
)" << "\033[0m\n";

    setConsoleColor(14); // Yellow
    std::cout << "<<<<< Welcome to the PHASE_C Chat Server >>>>>\n" << std::endl;
    setConsoleColor(7); // Default
}

void printClientConnected(const std::string &clientName) {
    setConsoleColor(10); // Green
    std::cout << "[+] New Client Connected: " << clientName << std::endl;
    setConsoleColor(7);
}

void printClientDisconnected(const std::string &clientName) {
    setConsoleColor(12); // Red
    std::cout << "[-] Client Disconnected: " << clientName << std::endl;
    setConsoleColor(7);
}

void printMessageReceived(const std::string &clientName, const std::string &message) {
    setConsoleColor(9); // Blue
    std::cout << "[" << clientName << "]: " << message << std::endl;
    setConsoleColor(7);
}

void printServerRunning(int port) {
    setConsoleColor(11); // Cyan
    std::cout << "Server is running on port: ";
    setConsoleColor(14); // Yellow
    std::cout << port << std::endl;
    setConsoleColor(7);
}

void printDivider() {
    setConsoleColor(8); // Gray
    std::cout << "------------------------------------------------------------" << std::endl;
    setConsoleColor(7);
}
void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            std::string clientName = serverUtils.getClientName(clientSocket);
            printClientDisconnected(clientName);
            serverUtils.removeClient(clientSocket);
            closesocket(clientSocket);
            break;
        }

        std::string message(buffer);
        std::string clientName = serverUtils.getClientName(clientSocket);
        printMessageReceived(clientName, message);

        FileUtils::appendToFile("chat_history.txt", clientName + ": " + message);

        // Broadcast message to other clients
        {
            std::lock_guard<std::mutex> lock(serverUtils.getClientMutex());
            for (auto &[socket, _] : serverUtils.getClientNames()) {
                if (socket != clientSocket) {
                    send(socket, message.c_str(), message.length(), 0);
                }
            }
        }
    }
}


int main() {
    printBanner();
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = {AF_INET, htons(PORT), INADDR_ANY};

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed!" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed!" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    crypto.generateKeys(3);
    printServerRunning(PORT);

    while (true) {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed!" << std::endl;
            continue;
        }

        char clientName[50] = {0};
        recv(clientSocket, clientName, sizeof(clientName), 0);
        serverUtils.registerClient(clientSocket, clientName);

        printClientConnected(clientName);
        printDivider();

        std::thread(handleClient, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
