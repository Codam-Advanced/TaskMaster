#pragma once

#include <signal.h>

namespace taskmasterd
{
enum class Signal
{
    INTERRUPT = SIGINT,
    TERMINATE = SIGTERM,
    KILL      = SIGKILL,
};
} // namespace taskmasterd
