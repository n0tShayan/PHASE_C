// A header file for the secure encryption code.



#ifndef CRYPTO_H
#define CRYPTO_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

class LatticeCrypto {
private:
    std::vector<std::vector<int>> publicKey;
    std::vector<std::vector<int>> privateKey;
    int modulus;

    std::vector<std::vector<int>> generateMatrix(int rows, int cols);
    std::vector<std::vector<int>> matrixMultiply(const std::vector<std::vector<int>> &A, const std::vector<std::vector<int>> &B);
    std::vector<std::vector<int>> addNoise(const std::vector<std::vector<int>> &matrix);

public:
    LatticeCrypto(int mod);
    void generateKeys(int size);
    std::vector<std::vector<int>> encrypt(const std::vector<int> &plaintext);
    std::vector<int> decrypt(const std::vector<std::vector<int>> &ciphertext);
    const std::vector<std::vector<int>> &getPublicKey() const;
};

#endif
