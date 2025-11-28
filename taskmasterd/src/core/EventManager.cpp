#include <taskmasterd/include/core/EventManager.hpp>

#include <stdexcept>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <logger/include/Logger.hpp>

namespace taskmasterd
{
EventManager::EventManager()
    : FileDescriptor(epoll_create1(0))
{
    if (_fd == -1) {
        throw std::runtime_error("Failed to create epoll file descriptor");
    }
}

void EventManager::registerEvent(const FileDescriptor& handler, EventCallback read_callback, EventCallback write_callback)
{
    this->updateEventInternal(handler, EPOLL_CTL_ADD, read_callback, write_callback);
}

void EventManager::updateEvent(const FileDescriptor& handler, EventCallback read_callback, EventCallback write_callback)
{
    this->updateEventInternal(handler, EPOLL_CTL_MOD, read_callback, write_callback);
}

void EventManager::updateEventInternal(const FileDescriptor& handler, i32 operation, EventCallback read_callback, EventCallback write_callback)
{
    struct epoll_event event{};
    // Set the events based on the provided callbacks
    if (read_callback)
        event.events = EPOLLIN;
    if (write_callback)
        event.events |= EPOLLOUT;
    event.data.fd = handler.getFd();

    if (epoll_ctl(_fd, operation, handler.getFd(), &event) == -1) {
        throw std::runtime_error("Failed to update file descriptor in epoll: " + std::string(strerror(errno)));
    }

    _read_callbacks[handler.getFd()]  = read_callback;
    _write_callbacks[handler.getFd()] = write_callback;
}

void EventManager::unregisterEvent(const FileDescriptor& handler)
{
    if (epoll_ctl(_fd, EPOLL_CTL_DEL, handler.getFd(), nullptr) == -1) {
        throw std::runtime_error("Failed to remove file descriptor from epoll: " + std::string(strerror(errno)));
    }

    _read_callbacks.erase(handler.getFd());
    _write_callbacks.erase(handler.getFd());
}

void EventManager::handleEvents()
{
    struct epoll_event events[MAX_EVENTS];

    i32 num_events = epoll_wait(_fd, events, MAX_EVENTS, -1);
    if (num_events == -1) {
        if (errno == EINTR)
            // Interrupted by signal, just return
            return;

        throw std::runtime_error("epoll_wait failed: " + std::string(strerror(errno)));
    }

    for (i32 i = 0; i < num_events; i++) {
        i32 fd = events[i].data.fd;
        try {
            if (events[i].events & EPOLLIN) {
                _read_callbacks.at(fd)();
            }
            if (events[i].events & EPOLLOUT) {
                _write_callbacks.at(fd)();
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Error handling event for fd " + std::to_string(fd) + ": " + e.what());
        }
    }
}

EventManager& EventManager::getInstance()
{
    static EventManager instance;

    return instance;
}
} // namespace taskmasterd
