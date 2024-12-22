//creating a client file for the code to handle multiple clients using Scoket programmming
// refernces for the code are https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
// ......
// ......
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <thread>


#pragma comment(lib, "ws2_32.lib")
using namespace std;
#define PORT 8080

// GUI Functions
void printBanner() {
// Updated ASCII Art for Server and Client with proper escape sequences
cout << "\033[1;36m" << R"(

  _____  _    _           _____ ______             _____ 
 |  __ \| |  | |   /\    / ____|  ____|           / ____|
 | |__) | |__| |  /  \  | (___ | |__             | |     
 |  ___/|  __  | / /\ \  \___ \|  __|            | |     
 | |    | |  | |/ ____ \ ____) | |____           | |____ 
 |_|    |_|  |_/_/    \_\_____/|______|           \_____|
                                         ______          
                                        |______|         
)" << "\033[0m\n";

    std::cout << "\033[1;33m<<<<< Welcome to the PHASE_C Chat Client >>>>>\033[0m\n" << std::endl;
}

void printInstructions() {
    std::cout << "\033[1;36mInstructions:\033[0m\n"
              << " - Type your messages and press Enter to send.\n"
              << " - Type 'exit' to leave the chat.\n"
              << "\033[1;30m------------------------------------------------------------\033[0m\n";
}

void printSentMessage(const std::string &message) {
    std::cout << "\033[1;32m[You]: \033[0m" << message << std::endl;
}

void printReceivedMessage(const std::string &message) {
    std::cout << "\033[1;34m" << message << "\033[0m" << std::endl;
}

void handle_server_response(SOCKET clientSocket) {
    char buffer[1024];
    int bytes_received;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            cout << "Disconnected from server.\n";
            break;
        }

        cout << "Server: " << buffer << endl;
    }
}



int main() {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        cerr << "WSAStartup failed.\n";
        return -1;
    }
    // initializing the socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed.\n";
        WSACleanup();
        return -1;
    }
    //setting up the socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Connection failed.\n";
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }
    //sucesfully connected
    cout << "Connected to server.\n";

    // Thread to handle server responses
    thread(handle_server_response, clientSocket).detach();
    //messade (bufer) stream
    char buffer[1024];
    while (true) {
        cout << "Enter message: ";
        cin.getline(buffer, sizeof(buffer));

        if (string(buffer) == "exit") {
            break;
        }

        send(clientSocket, buffer, strlen(buffer), 0);
    }
    //closing the socket
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}