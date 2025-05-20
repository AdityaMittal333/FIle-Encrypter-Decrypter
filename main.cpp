#include <iostream>
#include <filesystem>
#include <string>
#include <memory>

#include "processes/ProcessManagement.hpp"
#include "processes/Task.hpp"
#include "encryptDecrypt/CaesarCipher.hpp"
#include "encryptDecrypt/RSA.hpp"
#include "encryptDecrypt/XOR.hpp"

namespace fs = std::filesystem;

int main() {
    std::string directory;
    std::string actionStr;
    std::string algorithmStr;

    std::cout << "Enter the directory path: ";
    std::getline(std::cin, directory);

    std::cout << "Enter the action (encrypt/decrypt): ";
    std::getline(std::cin, actionStr);

    std::cout << "Enter the algorithm (caesar/rsa/xor): ";
    std::getline(std::cin, algorithmStr);

    // Parse action
    Action action;
    if (actionStr == "encrypt" || actionStr == "ENCRYPT") {
        action = Action::ENCRYPT;
    } else if (actionStr == "decrypt" || actionStr == "DECRYPT") {
        action = Action::DECRYPT;
    } else {
        std::cerr << "Invalid action. Use 'encrypt' or 'decrypt'." << std::endl;
        return 1;
    }

    // Parse algorithm
    Algorithm algorithm;
    if (algorithmStr == "caesar" || algorithmStr == "CAESAR") {
        algorithm = Algorithm::CAESAR;
    } else if (algorithmStr == "rsa" || algorithmStr == "RSA") {
        algorithm = Algorithm::RSA;
    } else if (algorithmStr == "xor" || algorithmStr == "XOR") {
        algorithm = Algorithm::XOR;
    } else {
        std::cerr << "Invalid algorithm. Use 'caesar', 'rsa', or 'xor'." << std::endl;
        return 1;
    }

    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        std::cerr << "Invalid directory path!" << std::endl;
        return 1;
    }

    ProcessManagement processManager;

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            IO io(filePath);
            std::fstream f_stream = std::move(io.getFileStream());

            if (!f_stream.is_open()) {
                std::cerr << "Unable to open file: " << filePath << std::endl;
                continue;
            }

            try {
                auto task = std::make_unique<Task>(std::move(f_stream), action, algorithm, filePath);
                if (!processManager.submitToQueue(std::move(task))) {
                    std::cerr << "Failed to submit task for file: " << filePath << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error creating task for file " << filePath << ": " << e.what() << std::endl;
            }
        }
    }

    return 0;
}