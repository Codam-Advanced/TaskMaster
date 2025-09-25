#include <logger/include/Logger.hpp>
#include <taskmasterd/include/example.hpp>

int main(int argc, char** argv)
{
    (void) argc;
    Logger::LogInterface::Initialize(argv[0], Logger::LogLevel::Debug, true);

    LOG_DEBUG("Debug log")
    LOG_ERROR("Error log")
    LOG_FATAL("Fatal log")
    LOG_WARNING("Warning log")
    LOG_INFO("Info log")

    return 0;
}
