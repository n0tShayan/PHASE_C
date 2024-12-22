#ifndef CRYPTO_H
#define CRYPTO_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <map>
#include <queue>
#include <stack>
#include <unordered_map>

// A class for implementing post-quantum cryptography with advanced data structures
class PostQuantumCrypto {
private:
    std::vector<std::vector<int>> publicKey;
    std::vector<std::vector<int>> privateKey;
    int modulus;

    // Graph representation for a lattice
    std::vector<std::vector<int>> adjacencyMatrix;

    // Generates a graph representing the lattice structure
    void generateLatticeGraph(int size);

    // Constructs a spanning tree from the graph
    std::vector<std::pair<int, int>> constructSpanningTree();

    // Matrix operations
    std::vector<std::vector<int>> generateMatrix(int rows, int cols);
    std::vector<std::vector<int>> matrixMultiply(const std::vector<std::vector<int>> &A, const std::vector<std::vector<int>> &B);
    std::vector<std::vector<int>> addNoise(const std::vector<std::vector<int>> &matrix);

public:
    PostQuantumCrypto(int mod);

    // Key generation
    void generateKeys(int size);

    // Encryption and decryption
    std::vector<std::vector<int>> encrypt(const std::vector<int> &plaintext);
    std::vector<int> decrypt(const std::vector<std::vector<int>> &ciphertext);

    // Getters
    const std::vector<std::vector<int>> &getPublicKey() const;
};

#endif