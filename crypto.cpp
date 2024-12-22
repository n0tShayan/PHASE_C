//Adding a file for Cryptographic Encryption
#include "crypto.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// Constructor
LatticeCrypto::LatticeCrypto(int mod) : modulus(mod) {
    if (modulus <= 0) {
        throw invalid_argument("Modulus must be positive");
    }
    std::srand(std::time(nullptr));
}

// Generates a random matrix of given dimensions
vector<vector<int>> LatticeCrypto::generateMatrix(int rows, int cols) {
    vector<vector<int>> matrix(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = std::rand() % modulus;
        }
    }
    return matrix;
}

// Multiplies two matrices
vector<vector<int>> LatticeCrypto::matrixMultiply(
    const vector<vector<int>> &A,
    const vector<vector<int>> &B) {
    int rowsA = A.size(), colsA = A[0].size(), colsB = B[0].size();
    vector<vector<int>> result(rowsA, vector<int>(colsB, 0));

    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsB; ++j) {
            for (int k = 0; k < colsA; ++k) {
                result[i][j] = (result[i][j] + A[i][k] * B[k][j]) % modulus;
            }
        }
    }
    return result;
}

// Adds random noise to a matrix
vector<vector<int>> LatticeCrypto::addNoise(
    const vector<vector<int>> &matrix) {
    vector<vector<int>> noisyMatrix = matrix;
    for (auto &row : noisyMatrix) {
        for (auto &value : row) {
            value = (value + std::rand() % 5 - 2 + modulus) % modulus; // Adds small noise
        }
    }
    return noisyMatrix;
}

// Generates public and private keys
void LatticeCrypto::generateKeys(int size) {
    privateKey = generateMatrix(size, size);
    publicKey = addNoise(matrixMultiply(privateKey, privateKey));
}
