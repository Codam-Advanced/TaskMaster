#include <logger/include/Logger.hpp>
#include <taskmasterd/include/example.hpp>
#include <iostream>

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "taskmasterd"
#endif

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;
    Logger::LogInterface::Initialize(PROGRAM_NAME, Logger::LogLevel::Debug, true);

    LOG_DEBUG("Debug log")
    LOG_ERROR("Error log")
    LOG_FATAL("Fatal log")
    LOG_WARNING("Warning log")
    LOG_INFO("Info log")

    return 0;
}
