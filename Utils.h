#pragma once
#include <exception>
#include <string>
#include <cstdint>

inline size_t convert_2d_to_1d_coordinates(size_t x, size_t y, size_t width)
{
    return x + y * width;
}

class Exception: public std::exception
{
public:
    Exception(const std::string& message): message{message}
    {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
private:
    std::string message;
};