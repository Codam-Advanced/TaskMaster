#include <Logger.hpp>
#include <iostream>

namespace Logger
{
void Log(const std::string& message)
{
    std::cout << message << std::endl;
}
} // namespace Logger
