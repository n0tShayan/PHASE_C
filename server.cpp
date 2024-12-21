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

// Lattice-based cryptographic system
class LatticeCrypto {
private:
    vector<vector<int>> publicKey;
    vector<vector<int>> privateKey;
    int modulus;

    // Generates a random matrix
    vector<vector<int>> generateMatrix(int rows, int cols) {
        vector<vector<int>> matrix(rows, vector<int>(cols));
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                matrix[i][j] = rand() % modulus;
        return matrix;
    }

    // Matrix multiplication with modular arithmetic
    vector<vector<int>> matrixMultiply(const vector<vector<int>> &A, const vector<vector<int>> &B) {
        int rows = A.size(), cols = B[0].size(), inner = B.size();
        vector<vector<int>> result(rows, vector<int>(cols, 0));

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                for (int k = 0; k < inner; ++k) {
                    result[i][j] = mod(result[i][j] + A[i][k] * B[k][j], modulus);
                }
            }
        }
        return result;
    }

    // Adds randomness to a matrix
    vector<vector<int>> addNoise(const vector<vector<int>> &matrix) {
        vector<vector<int>> noisyMatrix = matrix;
        for (auto &row : noisyMatrix) {
            for (auto &val : row) {
                val = mod(val + (rand() % 10), modulus); // Add small random noise
            }
        }
        return noisyMatrix;
    }

public:
    LatticeCrypto(int mod) : modulus(mod) {
        srand(time(0));
    }

    void generateKeys(int size) {
        privateKey = generateMatrix(size, size);
        publicKey = generateMatrix(size, size);
        cout << "Keys generated successfully.\n";
    }

    vector<vector<int>> encrypt(const vector<int> &plaintext) {
        vector<vector<int>> plainVec(plaintext.size(), vector<int>(1));
        for (int i = 0; i < plaintext.size(); ++i) {
            plainVec[i][0] = plaintext[i];
        }
        auto encrypted = matrixMultiply(publicKey, plainVec);
        return addNoise(encrypted); // Add randomness
    }

    vector<int> decrypt(const vector<vector<int>> &ciphertext) {
        vector<vector<int>> decryptedMatrix = matrixMultiply(privateKey, ciphertext);
        vector<int> plaintext;
        for (const auto &row : decryptedMatrix) {
            plaintext.push_back(row[0]);
        }
        return plaintext;
    }

    const vector<vector<int>>& getPublicKey() const {
        return publicKey;
    }
};

// Server-side code
vector<SOCKET> clients;
mutex clients_mutex;
LatticeCrypto crypto(101);

void sendPublicKey(SOCKET client_socket) {
    const auto& publicKey = crypto.getPublicKey();
    for (const auto& row : publicKey) {
        for (int val : row) {
            send(client_socket, reinterpret_cast<const char*>(&val), sizeof(val), 0);
        }
    }
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

        cout << "Received message: " << buffer << endl;

        // Echo the message back to the client
        lock_guard<mutex> lock(clients_mutex);
        for (SOCKET client : clients) {
            if (client != client_socket) {
                send(client, buffer, bytes_received, 0);
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

        cout << "New client connected. Sending public key.\n";
        sendPublicKey(client_socket);

        thread(handle_client, client_socket).detach();
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
