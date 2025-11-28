#pragma once

#include <ostream>
#include <signal.h>
#include <sstream>

namespace taskmasterd
{
enum class Signals : int
{
    INT  = SIGINT,
    TERM = SIGTERM,
    HUP  = SIGHUP,
    QUIT = SIGQUIT,
    KILL = SIGKILL,
    USR1 = SIGUSR1,
    USR2 = SIGUSR2
};

std::ostream& operator<<(std::ostream& os, Signals signal);

std::string to_string(Signals signal);
} // namespace taskmasterd
