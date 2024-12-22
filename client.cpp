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