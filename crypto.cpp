#include "crypto.h"
#include <iostream>
#include <stdexcept>

PostQuantumCrypto::PostQuantumCrypto(int mod) : modulus(mod) {
    if (modulus <= 0) {
        throw std::invalid_argument("Modulus must be positive");
    }
    std::srand(std::time(nullptr));
}

void PostQuantumCrypto::generateLatticeGraph(int size) {
    adjacencyMatrix.resize(size, std::vector<int>(size, 0));
    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            int weight = std::rand() % modulus;
            adjacencyMatrix[i][j] = weight;
            adjacencyMatrix[j][i] = weight;
        }
    }
}

std::vector<std::pair<int, int>> PostQuantumCrypto::constructSpanningTree() {
    int n = adjacencyMatrix.size();
    std::vector<bool> visited(n, false);
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, std::greater<>> pq;

    // Start from the first node
    visited[0] = true;
    for (int i = 1; i < n; ++i) {
        if (adjacencyMatrix[0][i] > 0) {
            pq.push({adjacencyMatrix[0][i], {0, i}});
        }
    }

    std::vector<std::pair<int, int>> spanningTree;

    while (!pq.empty()) {
        auto [weight, edge] = pq.top();
        pq.pop();

        int u = edge.first, v = edge.second;
        if (!visited[v]) {
            visited[v] = true;
            spanningTree.push_back({u, v});

            for (int i = 0; i < n; ++i) {
                if (!visited[i] && adjacencyMatrix[v][i] > 0) {
                    pq.push({adjacencyMatrix[v][i], {v, i}});
                }
            }
        }
    }

    return spanningTree;
}

std::vector<std::vector<int>> PostQuantumCrypto::generateMatrix(int rows, int cols) {
    std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = std::rand() % modulus;
        }
    }
    return matrix;
}

std::vector<std::vector<int>> PostQuantumCrypto::matrixMultiply(const std::vector<std::vector<int>> &A, const std::vector<std::vector<int>> &B) {
    int rowsA = A.size(), colsA = A[0].size(), colsB = B[0].size();
    std::vector<std::vector<int>> result(rowsA, std::vector<int>(colsB, 0));

    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsB; ++j) {
            for (int k = 0; k < colsA; ++k) {
                result[i][j] = (result[i][j] + A[i][k] * B[k][j]) % modulus;
            }
        }
    }
    return result;
}

std::vector<std::vector<int>> PostQuantumCrypto::addNoise(const std::vector<std::vector<int>> &matrix) {
    std::vector<std::vector<int>> noisyMatrix = matrix;
    for (auto &row : noisyMatrix) {
        for (auto &value : row) {
            value = (value + std::rand() % 5 - 2 + modulus) % modulus;
        }
    }
    return noisyMatrix;
}

void PostQuantumCrypto::generateKeys(int size) {
    generateLatticeGraph(size);
    auto spanningTree = constructSpanningTree();

    privateKey = generateMatrix(size, size);
    publicKey = addNoise(matrixMultiply(privateKey, privateKey));
}

std::vector<std::vector<int>> PostQuantumCrypto::encrypt(const std::vector<int> &plaintext) {
    std::vector<std::vector<int>> plaintextMatrix(1, plaintext);
    return matrixMultiply(plaintextMatrix, publicKey);
}

std::vector<int> PostQuantumCrypto::decrypt(const std::vector<std::vector<int>> &ciphertext) {
    std::vector<std::vector<int>> decryptedMatrix = matrixMultiply(ciphertext, privateKey);
    return decryptedMatrix[0];
}

const std::vector<std::vector<int>> &PostQuantumCrypto::getPublicKey() const {
    return publicKey;
}