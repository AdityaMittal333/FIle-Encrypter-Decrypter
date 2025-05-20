#include "CaesarCipher.hpp"
#include "../processes/Task.hpp"
#include <iostream>

int shiftChar(char c, int shift) {
    if (c >= 'a' && c <= 'z') {
        return (c - 'a' + shift + 26) % 26 + 'a';
    } else if (c >= 'A' && c <= 'Z') {
        return (c - 'A' + shift + 26) % 26 + 'A';
    }
    return c;
}

int executeCaesarCipher(const std::string& taskData) {
    Task task = Task::fromString(taskData);

    int shift = 3; // classic Caesar shift
    int actualShift = (task.action == Action::ENCRYPT) ? shift : -shift;

    std::string buffer;
    char ch;

    while (task.f_stream.get(ch)) {
        buffer.push_back(shiftChar(ch, actualShift));
    }

    task.f_stream.clear();
    task.f_stream.seekp(0, std::ios::beg);

    for (char c : buffer) {
        task.f_stream.put(c);
    }

    task.f_stream.close();
    return 0;
}
