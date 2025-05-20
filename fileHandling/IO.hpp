#ifndef IO_HPP
#define IO_HPP

#include <fstream>
#include <string>

class IO {
public:
    explicit IO(const std::string& file_path);
    ~IO();
    
    // Returns a movable fstream for read/write in binary mode
    std::fstream getFileStream();

private:
    std::fstream file_stream;
};

#endif // IO_HPP
