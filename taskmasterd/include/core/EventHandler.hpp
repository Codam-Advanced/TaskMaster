#pragma once

#include <utils/include/utils.hpp>

namespace taskmasterd
{
enum class EventType : u32
{
    READ  = 0x01,
    WRITE = 0x04,
};

class EventHandler
{
public:
    EventHandler(i32 fd) : _fd(fd) {}
    virtual ~EventHandler() = default;

    virtual void handleRead();
    virtual void handleWrite();

    i32 getFd() const { return _fd; }

protected:
    i32 _fd;
};
} // namespace taskmasterd
