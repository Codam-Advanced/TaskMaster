#include <taskmasterd/include/core/EventManager.hpp>

#include <iostream>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>

#include <logger/include/Logger.hpp>

namespace taskmasterd
{
EventManager::EventManager() : _epoll_fd(epoll_create1(0))
{
    std::cout << "EventManager created with epoll fd: " << _epoll_fd << std::endl;
    if (_epoll_fd == -1) {
        throw std::runtime_error("Failed to create epoll file descriptor");
    }
}

EventManager::~EventManager()
{
    if (_epoll_fd != -1) {
        close(_epoll_fd);
    }
}

void EventManager::registerEvent(EventHandler* handler, EventType type)
{
    struct epoll_event event;
    event.events   = static_cast<u32>(type) | EPOLLET;
    event.data.ptr = handler;

    LOG_DEBUG("Registering event for fd " + std::to_string(handler->getFd()));

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, handler->getFd(), &event) == -1) {
        throw std::runtime_error("Failed to add file descriptor to epoll");
    }
}

void EventManager::updateEvent(EventHandler* handler, EventType type)
{
    struct epoll_event event;
    event.events   = static_cast<u32>(type) | EPOLLET;
    event.data.ptr = handler;

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, handler->getFd(), &event) == -1) {
        throw std::runtime_error("Failed to modify file descriptor in epoll");
    }
}

void EventManager::unregisterEvent(EventHandler* handler)
{
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, handler->getFd(), nullptr) == -1) {
        throw std::runtime_error("Failed to remove file descriptor from epoll");
    }
}

void EventManager::handleEvents()
{
    struct epoll_event events[MAX_EVENTS];

    i32 num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
    if (num_events == -1) {
        throw std::runtime_error("epoll_wait failed");
    }

    for (i32 i = 0; i < num_events; i++) {
        EventHandler* handler = static_cast<EventHandler*>(events[i].data.ptr);
        if (events[i].events & EPOLLIN) {
            handler->handleRead();
        }
        if (events[i].events & EPOLLOUT) {
            handler->handleWrite();
        }
    }
}

void EventManager::initialize()
{
    auto& instance = getInstance();
    if (instance) {
        throw std::runtime_error("EventManager is already initialized");
    }

    instance = std::make_unique<EventManager>();
}

std::unique_ptr<EventManager>& EventManager::getInstance()
{
    static std::unique_ptr<EventManager> instance;

    return instance;
}
} // namespace taskmasterd
