#include <taskmasterd/include/core/EventHandler.hpp>

namespace taskmasterd
{
EventHandler::EventHandler(i32 fd, Callback readCallback, Callback writeCallback)
    : _fd(fd), _readCallback(readCallback), _writeCallback(writeCallback)
{
}

void EventHandler::setFd(i32 fd)
{
    _fd = fd;
}

void EventHandler::handleRead()
{
    if (_readCallback)
        _readCallback();
}

void EventHandler::handleWrite()
{
    if (_writeCallback)
        _writeCallback();
}
} // namespace taskmasterd
