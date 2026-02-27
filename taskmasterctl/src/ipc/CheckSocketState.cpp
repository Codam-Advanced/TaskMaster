#include <taskmasterctl/include/ipc/CheckSocketState.hpp>
#include <ipc/include/Socket.hpp>
#include <logger/include/Logger.hpp>
#include <sys/epoll.h>
#include <unistd.h>

constexpr i32 MAX_EVENTS = 16;
constexpr i32 TIMEOUT = 1000;

namespace taskmasterctl
{

void checkSocketState(const ipc::Socket& socket)
{
    struct epoll_event events[MAX_EVENTS];
    struct epoll_event event;
    auto               epollFd = epoll_create1(0);

    if (epollFd == -1) {
        throw std::runtime_error("Failed to create the epoll file descriptor: " + std::string(strerror(errno)));
    }

    // Set the event for socket closure
    event.events = EPOLLHUP;
    event.data.fd = socket.getFd();
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, socket.getFd(), &event) == -1) {
        throw std::runtime_error("Failed to update file descriptor in epoll: " + std::string(strerror(errno)));
    }

    while (g_exitChecker == false) {
        i32 num_events = epoll_wait(epollFd, events, MAX_EVENTS, TIMEOUT);

        if (num_events == -1) {
            // Interrupted by signal, just return
            if (errno == EINTR)
                return;
            throw std::runtime_error("epoll_wait failed: " + std::string(strerror(errno)));
        }

        for (i32 i = 0; i < num_events; i++) {
            if (events[i].events & EPOLLHUP) {
                LOG_INFO("\nDeamon has closed the socket");

                // Raise a SIGINT so the main thread gets interupted

                // If a terminate command has been sent then the main thread is already waiting
                // for this thread to end, no need to raise a signal as this will set an error exit code
                // and prevent proper cleanup of resources.
                if (g_exitChecker == false) 
                    raise(SIGINT);
                return ;
            }
        }
    }
}

} // namespace taskmasterctl
