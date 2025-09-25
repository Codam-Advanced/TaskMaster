#pragma once

#include <functional>

#include <taskmasterd/include/core/EventHandler.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
class Timer : public EventHandler
{
public:
    Timer(i32 interval, std::function<void()> callback);
    virtual ~Timer();

    void start();

    void handleRead() override;

private:
    i32 _interval;

    std::function<void()> _callback;
};
} // namespace taskmasterd
