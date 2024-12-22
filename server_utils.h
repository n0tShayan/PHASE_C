
#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <string>
#include <unordered_map>
#include <vector>
#include <winsock2.h>
#include <mutex>

class ServerUtils {
private:
    std::unordered_map<SOCKET, std::string> clientNames;
    std::mutex clientMutex;

public:
    void registerClient(SOCKET clientSocket, const std::string &name);
    std::string getClientName(SOCKET clientSocket);
    void removeClient(SOCKET clientSocket);

    std::unordered_map<SOCKET, std::string> &getClientNames();
    std::mutex &getClientMutex();
};

#endif