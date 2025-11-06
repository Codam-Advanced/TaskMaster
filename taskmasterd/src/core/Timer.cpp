#include <taskmasterd/include/core/Timer.hpp>

#include <stdexcept>
#include <sys/timerfd.h>
#include <unistd.h>

#include <logger/include/Logger.hpp>
#include <taskmasterd/include/core/EventManager.hpp>

namespace taskmasterd
{
Timer::Timer(i32 interval, std::function<void()> callback)
    : _interval(interval)
    , _state(State::STOPPED)
    , _callback(callback)
{
    _fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (_fd == -1) {
        throw std::runtime_error("Failed to create timerfd");
    }
}

Timer::~Timer()
{
    if (_state == State::RUNNING) {
        EventManager::getInstance().unregisterEvent(*this);
    }
}

void Timer::start()
{
    struct itimerspec new_value;
    new_value.it_value.tv_sec     = _interval;
    new_value.it_value.tv_nsec    = 0;
    new_value.it_interval.tv_sec  = _interval;
    new_value.it_interval.tv_nsec = 0;

    if (timerfd_settime(_fd, 0, &new_value, NULL) == -1) {
        throw std::runtime_error("Failed to set timerfd time");
    }

    EventManager::getInstance().registerEvent(*this, std::bind(&Timer::onExpire, this), nullptr);

    _state = State::RUNNING;
}

void Timer::onExpire()
{
    _state = State::STOPPED;

    u64     expirations;
    ssize_t s = read(_fd, &expirations, sizeof(expirations));
    if (s != sizeof(expirations)) {
        throw std::runtime_error("Failed to read timerfd");
    }

    LOG_DEBUG("Timer expired " + std::to_string(expirations) + " times");

    _callback();
}
} // namespace taskmasterd
