#pragma once

#include <memory>

#include <taskmasterd/include/core/EventHandler.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
class EventManager
{
public:
    EventManager();
    virtual ~EventManager();

    void registerEvent(EventHandler* handler, EventType type);
    void handleEvents();

    static std::unique_ptr<EventManager>& getInstance();

private:
    i32 _epoll_fd;

    const static i32 MAX_EVENTS = 10;
};
} // namespace taskmasterd
