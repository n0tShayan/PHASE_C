server_utils.cpp:
#include "server_utils.h"

void ServerUtils::registerClient(SOCKET clientSocket, const std::string &name) {
    std::lock_guard<std::mutex> lock(clientMutex);
    clientNames[clientSocket] = name;
}

std::string ServerUtils::getClientName(SOCKET clientSocket) {
    std::lock_guard<std::mutex> lock(clientMutex);
    return clientNames[clientSocket];
}

