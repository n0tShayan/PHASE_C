//creating a client file for the code to handle multiple clients using Scoket programmming
// refernces for the code are https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
// ......
// ......
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <thread>



int main() {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        cerr << "WSAStartup failed.\n";
        return -1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed.\n";
        WSACleanup();
        return -1;
    }

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

    cout << "Connected to server.\n";

    // Thread to handle server responses
    thread(handle_server_response, clientSocket).detach();

    char buffer[1024];
    while (true) {
        cout << "Enter message: ";
        cin.getline(buffer, sizeof(buffer));

        if (string(buffer) == "exit") {
            break;
        }

        send(clientSocket, buffer, strlen(buffer), 0);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}