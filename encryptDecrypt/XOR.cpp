#include "XOR.hpp"
#include "../processes/Task.hpp"
#include <iostream>

int executeXOR(const std::string& taskData) {
    Task task = Task::fromString(taskData);

    const char key = 0xAA; // Simple fixed key

    std::string buffer;
    char ch;

    while (task.f_stream.get(ch)) {
        buffer.push_back(ch ^ key);
    }

    task.f_stream.clear();
    task.f_stream.seekp(0, std::ios::beg);

    for (char c : buffer) {
        task.f_stream.put(c);
    }

    task.f_stream.close();
    return 0;
}
