#ifndef TASK_HPP
#define TASK_HPP

#include "../fileHandling/IO.hpp"
#include <fstream>
#include <string>
#include <sstream>

enum class Action {
    ENCRYPT,
    DECRYPT
};

enum class Algorithm {
    CAESAR,
    RSA,
    XOR
};

struct Task {
    std::string filePath;
    std::fstream f_stream;
    Action action;
    Algorithm algorithm;

    Task(std::fstream&& stream, Action act, Algorithm algo, std::string filePath) 
        : f_stream(std::move(stream)), action(act), algorithm(algo), filePath(std::move(filePath)) {}

    // Serialize task to string format: filePath|algorithm|action
    std::string toString() const {
        std::ostringstream oss;
        oss << filePath << "|" 
            << (algorithm == Algorithm::CAESAR ? "CAESAR" : (algorithm == Algorithm::RSA ? "RSA" : "XOR")) << "|" 
            << (action == Action::ENCRYPT ? "ENCRYPT" : "DECRYPT");
        return oss.str();
    }

    // Deserialize task from string
    static Task fromString(const std::string& taskData) {
        std::istringstream iss(taskData);
        std::string filePath, algoStr, actionStr;

        if (getline(iss, filePath, '|') && getline(iss, algoStr, '|') && getline(iss, actionStr)) {
            Action action = (actionStr == "ENCRYPT") ? Action::ENCRYPT : Action::DECRYPT;
            Algorithm algorithm;
            if (algoStr == "CAESAR") algorithm = Algorithm::CAESAR;
            else if (algoStr == "RSA") algorithm = Algorithm::RSA;
            else algorithm = Algorithm::XOR;

            IO io(filePath);
            std::fstream f_stream = std::move(io.getFileStream());
            if (!f_stream.is_open()) {
                throw std::runtime_error("Failed to open file: " + filePath);
            }

            return Task(std::move(f_stream), action, algorithm, filePath);
        } else {
            throw std::runtime_error("Invalid task data format");
        }
    }
};

#endif // TASK_HPP
