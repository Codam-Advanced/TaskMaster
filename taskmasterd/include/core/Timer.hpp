#pragma once

#include <functional>

#include <ipc/include/FileDescriptor.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
class Timer : public FileDescriptor
{
public:
    enum class State
    {
        STOPPED,
        RUNNING
    };

    /**
     * @brief Construct a new Timer object.
     *
     * @param interval The timer interval in seconds.
     * @param callback The callback function to be called when the timer expires.
     */
    Timer(i32 interval, std::function<void()> callback);
    virtual ~Timer();

    /**
     * @brief Start the timer.
     *
     * This method sets the timer to expire after the specified interval
     * and registers it with the EventManager to handle read events.
     */
    void start();

    /**
     * @brief Callback function for timer expiration.
     *
     * This method is called when the timer expires. It reads the expiration count
     * from the timer file descriptor and invokes the callback function.
     */
    void onExpire();

private:
    i32   _interval;
    State _state;

    std::function<void()> _callback;
};
} // namespace taskmasterd
