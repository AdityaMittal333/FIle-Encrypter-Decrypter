#include "IO.hpp"
#include <iostream>

IO::IO(const std::string& file_path) {
    // Open file for both input and output in binary mode
    file_stream.open(file_path, std::ios::in | std::ios::out | std::ios::binary);
    if (!file_stream.is_open()) {
        std::cerr << "Unable to open file: " << file_path << std::endl;
    }
}

IO::~IO() {
    if (file_stream.is_open()) {
        file_stream.close();
    }
}

std::fstream IO::getFileStream() {
    // Return stream by moving ownership
    return std::move(file_stream);
}
