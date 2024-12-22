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

    cout << "\033[1;33m<<<<< Welcome to the PHASE_C Chat Client >>>>>\033[0m\n" << endl;
}

void printInstructions() {
    cout << "\033[1;36mInstructions:\033[0m\n"
              << " - Type your messages and press Enter to send.\n"
              << " - Type 'exit' to leave the chat.\n"
              << "\033[1;30m------------------------------------------------------------\033[0m\n";
}

void printSentMessage(const string &message) {
    cout << "\033[1;32m[You]: \033[0m" << message << endl;
}

void printReceivedMessage(const string &message) {
    cout << "\033[1;34m" << message << "\033[0m" << endl;
}

void handleServerResponse(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "\033[1;31mDisconnected from server.\033[0m" << endl;
            break;
        }
        printReceivedMessage(buffer);
    }
}



int main() {
    printBanner();
    printInstructions();

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr = {AF_INET, htons(PORT), 0};
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));

    cout << "Enter your name: ";
    string name;
    getline(cin, name);
    send(clientSocket, name.c_str(), name.length(), 0);

    thread(handleServerResponse, clientSocket).detach();

    char buffer[1024];
    while (true) {
        cin.getline(buffer, sizeof(buffer));
        if (string(buffer) == "exit") {
            break;
        }
        send(clientSocket, buffer, strlen(buffer), 0);
        printSentMessage(buffer);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}