#ifndef READENV_HPP
#define READENV_HPP

#include <string>

class ReadEnv {
public:
    // Reads entire content of ".env" file as a string
    std::string getenv();
};

#endif // READENV_HPP
