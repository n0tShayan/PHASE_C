#include "server_utils.h"

void ServerUtils::registerClient(SOCKET clientSocket, const std::string &name) {
    std::lock_guard<std::mutex> lock(clientMutex);
    clientNames[clientSocket] = name;
}

std::string ServerUtils::getClientName(SOCKET clientSocket) {
    std::lock_guard<std::mutex> lock(clientMutex);
    return clientNames[clientSocket];
}
void ServerUtils::removeClient(SOCKET clientSocket) {
    std::lock_guard<std::mutex> lock(clientMutex);
    clientNames.erase(clientSocket);
}

std::unordered_map<SOCKET, std::string> &ServerUtils::getClientNames() {
    return clientNames;
}

std::mutex &ServerUtils::getClientMutex() {
    return clientMutex;
}