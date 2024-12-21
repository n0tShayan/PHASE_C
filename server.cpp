#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

vector<SOCKET> clients;
mutex clients_mutex;

// Utility function for modular arithmetic
int mod(int a, int b) {
    return (a % b + b) % b;
}

void handle_client(SOCKET client_socket) {
    char buffer[1024];
    int bytes_received;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            lock_guard<mutex> lock(clients_mutex);
            clients.erase(remove(clients.begin(), clients.end(), client_socket), clients.end());
            closesocket(client_socket);
            cout << "Client disconnected.\n";
            break;
        }

        cout << "Message received: " << buffer << endl;

        // Broadcast message to all other clients
        lock_guard<mutex> lock(clients_mutex);
        for (SOCKET client : clients) {
            if (client != client_socket) {
                if (send(client, buffer, bytes_received, 0) == SOCKET_ERROR) {
                    cerr << "Error sending message to a client.\n";
                }
            }
        }
    }
}

int main() {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        cerr << "WSAStartup failed.\n";
        return 1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        cerr << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Bind failed.\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed.\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    cout << "Server started on port 8080...\n";

    while (true) {
        sockaddr_in client_addr;
        int client_size = sizeof(client_addr);
        SOCKET client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_size);

        if (client_socket == INVALID_SOCKET) {
            cerr << "Accept failed.\n";
            continue;
        }

        {
            lock_guard<mutex> lock(clients_mutex);
            clients.push_back(client_socket);
        }

        cout << "New client connected.\n";
        thread(handle_client, client_socket).detach();
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
