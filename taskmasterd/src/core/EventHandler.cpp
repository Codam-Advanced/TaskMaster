#include <taskmasterd/include/core/EventHandler.hpp>

#include <logger/include/Logger.hpp>

namespace taskmasterd
{
EventHandler::EventHandler(EventHandler&& other) noexcept : _fd(other._fd)
{
    other._fd = -1;
}

EventHandler::~EventHandler()
{
    if (_fd != -1) {
        close(_fd);
    }
}
} // namespace taskmasterd