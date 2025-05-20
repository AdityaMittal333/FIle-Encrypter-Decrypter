#include "ReadEnv.hpp"
#include "IO.hpp"
#include <sstream>

std::string ReadEnv::getenv() {
    const std::string env_path = ".env";
    IO io(env_path);
    std::fstream f_stream = io.getFileStream();

    if (!f_stream.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << f_stream.rdbuf();
    return buffer.str();
}
