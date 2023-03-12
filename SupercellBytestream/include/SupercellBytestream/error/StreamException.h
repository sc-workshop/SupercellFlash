#pragma once 

#include <string>

#include <sstream>

enum class StreamError {
    UNKNOWN = -1,
    EXIST_ERROR = 1,
    CLOSED_ERROR,
    WRITE_ERROR,
    READ_ERROR,
};

struct StreamException : public std::exception {
    StreamError code;
    std::string message;
    StreamException(StreamError code, std::string message) : code(code), message(message) {};

    const char* what() {
        std::stringstream s;
        s << message << std::endl;
        s << "Exception code: " << (uint8_t)code << std::endl;

        return s.str().c_str();
    }
};