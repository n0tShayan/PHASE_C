#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <fstream>
#include <string>

class FileUtils {
public:
    static void appendToFile(const std::string &filename, const std::string &data) {
        std::ofstream file(filename, std::ios::app);
        if (file.is_open()) {
            file << data << std::endl;
            file.close();
        }
    }
};

#endif